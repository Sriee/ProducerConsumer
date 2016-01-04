# ProducerConsumer
Synchronization problem - Producer-Consumer

<b>Objective</b><br> 

The program should create separate threads for Producers and Consumers. Each Producer Thread can create random numbers up to maximum buffer limit. The created random numbers are stored in a text file <b>“Input-numbers.txt”</b>. Each Consumer thread should read random numbers one by one from the text file and check whether the number is prime or not. If the number is prime then that number is stored in <b>“Prime.txt”</b> file, if not the number is stored in <b>“Non-Prime.txt”</b>.<br>

<b>Input Format</b><br>

<b>./ProgramName.out m n p q</b><br>

Where <br>
m - Number of Producers <br>
n - Number of Consumers <br>
p - Maximum buffer limit <br>
q - Number of random numbers each Producer can create <br>

<b>Conditions</b><br>

1. Each random number should be read exactly one time from Input-number.txt <br>
2. Consumer thread should wait if the producer has not created any numbers. <br>
3. If earlier consumer threads have processed all the numbers before other consumer threads starts processing,
   remaining consumer threads should be terminated. <br>
4. Number of Producer and Consumer thread is specified in the arguments should be running. <br>
5. Producer thread should produce numbers up to the buffer limit.<br> 
6. When the producer has reached the maximum buffer limit, it should wait for the consumer to process the created numbers.<br> 

 

