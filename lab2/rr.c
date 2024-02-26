#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;
  int last_process_time;

  TAILQ_ENTRY(process) pointers;
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
    (*process_data)[i].last_process_time = -1;
  }

  munmap((void *)data, size);
  close(fd);
}

void sort_processes_by_arrival_time(struct process *process_data, u32 process_size)
{
  for (u32 i = 0; i < process_size - 1; ++i)
  {
    for (u32 j = 0; j < process_size - i - 1; ++j)
    {
      if (process_data[j].arrival_time > process_data[j + 1].arrival_time)
      {
        struct process temp = process_data[j];
        process_data[j] = process_data[j + 1];
        process_data[j + 1] = temp;
      }
    }
  }
}

void print_processes(struct process *process_data, u32 process_size)
{
  printf("PID\tArrival current_time\tBurst current_time\n");
  for (u32 i = 0; i < process_size; ++i)
  {
    printf("%u\t%u\t\t%u\n", process_data[i].pid, process_data[i].arrival_time, process_data[i].burst_time);
  }
}

void sort_arrival_time(struct process *data, u32 size)
{
  for (u32 i = 0; i < size - 1; ++i)
  {
    for (u32 j = 0; j < size - i - 1; ++j)
    {
      if (data[j].arrival_time > data[j + 1].arrival_time)
      {
        struct process temp = data[j];
        data[j] = data[j + 1];
        data[j + 1] = temp;
      }
    }
  }
}

void process_round_robin(struct process_list *list, struct process *data, u32 size, u32 quantum_length, u32 *total_waiting_time, u32 *total_response_time)
{
  struct process *current_process = NULL;
  u32 idx = 0;
  u32 standby_switch = 0;
  u32 current_time = 0; 
  
  while (idx < size || !TAILQ_EMPTY(list))
  {
    u32 next_step = standby_switch + quantum_length;
    while (data[idx].arrival_time <= current_time && idx < size)
    {
      TAILQ_INSERT_TAIL(list, &data[idx], pointers);
      idx++;
    }

    if (current_process == NULL || next_step <= current_time)
    {
      standby_switch = current_time;
      if (current_process != NULL)
      {
        TAILQ_INSERT_TAIL(list, current_process, pointers);
        current_process = NULL;
      }

      if (!TAILQ_EMPTY(list))
      {
        current_process = TAILQ_FIRST(list);
        TAILQ_REMOVE(list, current_process, pointers);
        if (current_process->last_process_time >= 0)
        {
          *total_waiting_time += current_time - current_process->last_process_time;
        }
        else
        {
          *total_response_time += current_time - current_process->arrival_time;
          *total_waiting_time += current_time - current_process->arrival_time;
        }
      }
    }
    current_time++;

    if (current_process != NULL)
    {
      current_process->burst_time -= 1;
      current_process->last_process_time = current_time;
      if(current_process->burst_time <= 0)
        current_process = NULL;
    }
  }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return EINVAL;
    }
    
    struct process *data;
    u32 size;
    init_processes(argv[1], &data, &size);

    u32 quantum_length = next_int_from_c_str(argv[2]);

    struct process_list list;
    TAILQ_INIT(&list);

    u32 total_waiting_time = 0;
    u32 total_response_time = 0;

    sort_arrival_time(data, size);

    process_round_robin(&list, data, size, quantum_length, &total_waiting_time, &total_response_time);

    printf("Average waiting current_time: %.2f\n", (float)total_waiting_time / (float)size);
    printf("Average response current_time: %.2f\n", (float)total_response_time / (float)size);

    free(data);
    return 0;
}
