# Fair solution of Starve-free-Readers-Writers Problem
### Instructions to run the code in ``code.cpp``
###### Compilation command:
```
g++ -pthreads code.cpp
```
###### Execution Command:
```
./a.out
```
### Pseudocode descripton

###### Variable Descriptions
- **begin**: This semaphore allows only one process at a time to execute ENTRY section for readers, and protects readers from entering while writer is in CRITICAL section
- **finish**: This semaphore allows only one reader process to execute EXIT section.
- **writer_wait**: This semaphore is allows waiting writer to get priority if readers are in CRITICAL section.
- **reader_enter**: This integer variable stores the number of reader processes that executed ENTRY section.
- **reader_exit**: This integer variable stores the number of reader processes that executed the EXIT section.
- **writer_waiting**: This boolean variable stores whether any writer is waiting while readers are in CRITICAL section.
- **DATA**: Global shared data. Here it is integer type for simplicity.

#### Reader function
- **ENTRY SECTION**: Takes control of ``begin`` semaphore to enter ENTRY section if possible, otherwise wait till it gets control. Increment the ``reader_enter`` variable. Release the ``begin`` semaphotre.
- **CRITICAL SECTION**: Read the ``DATA``.
- **EXIT SECTION**: Take control of ``finish`` semaphore to enter EXIT section if possible, otherwise wait till it gets control. Increment the ``reader_exit`` variable. Check if a writer process is waiting and no other reader process is in CRITICAL SECTION, then Signal the ``writer_wait`` semaphore. Finally, release the ``finish`` semaphore.

#### Writer function
- **ENTRY SECTION**: Take control of ``begin`` semaphore and then ``finish`` semaphore, if possible, otherwise wait till it gets control. This ensure no reader process is in ENTRY and EXIT sections. Also, check if no Reader process is in CRITICAL SECTION. If true, Release ``finish`` semaphore. Otherwise, Update the ``writer_waiting`` variable to true, to indicate to reader processes that a writer process is waiting; Also, release ``finish`` semaphore to allow reader processes in CRITICAL section to exit; Wait till ``writer_wait`` semaphore is taken to control; Also update ``writer_waiting`` to false.

- **CRITICAL SECTION**: Process or Modify ``DATA``

- **EXIT SECTION**: Release the semaphore ``begin``, to allow other readers and writers process to enter.
