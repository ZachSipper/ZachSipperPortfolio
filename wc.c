#include "wc.h"
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
		long fsize;
		FILE *fp;
		count_t total, count, buf;
		struct timespec begin, end;
		int nChildProc = 1;
		
		/* 1st arg: filename */
		if(argc < 2) {
				printf("usage: wc <filname> [# processes] [crash rate]\n");
				return 0;
		}
		
		/* 2nd (optional) arg: number of child processes */
		if (argc > 2) {
				nChildProc = atoi(argv[2]);
				if(nChildProc < 1) nChildProc = 1;
				if(nChildProc > 10) nChildProc = 10;
		}

		/* 3rd (optional) arg: crash rate between 0% and 100%. Each child process has that much chance to crash*/
		if(argc > 3) {
				crashRate = atoi(argv[3]);
				if(crashRate < 0) crashRate = 0;
				if(crashRate > 50) crashRate = 50;
				printf("crashRate RATE: %d\n", crashRate);
		}
		
		printf("# of Child Processes: %d\n", nChildProc);
		printf("crashRate RATE: %d\%\n", crashRate);

		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;

		// start to measure time
		clock_gettime(CLOCK_REALTIME, &begin);

		// Open file in read-only mode
		fp = fopen(argv[1], "r");

		if(fp == NULL) {
				printf("File open error: %s\n", argv[1]);
				printf("usage: wc <filname>\n");
				return 0;
		}
		
		// get a file size
		fseek(fp, 0L, SEEK_END);
		fsize = ftell(fp);		
		fclose(fp);

		int pArray[nChildProc][2], pid, pidArray[nChildProc], i, j;
		double delegated_size = ceil((double) fsize / nChildProc);
	        
		for(i = 0; i < nChildProc; i++){ // loop to create the desired number of processes
		  
		  if(pipe(pArray[i]) != 0) exit(1); // create pipe and exit if there is a pipe error

		  pid = fork(); // create child process
		  pidArray[i] = pid;
		  
		  if(pid < 0){

		    printf("Fork faild.\n");
		    exit(1);

		  } // if
		  
		  else if(pid == 0){ // CHILD
		    
		    //Do child stuff
		    fp = fopen(argv[1], "r");
		    
		    // Edge case: this makes sure the read size for the last child is correct
		    if((i+1) == nChildProc && ((int) fsize % (int) delegated_size) != 0){
		      count = word_count(fp, delegated_size * i, ((int) fsize % (int) delegated_size));
		    }else{
		      count = word_count(fp, delegated_size * i, delegated_size);
		    } // else
		      
		    fclose(fp);

		    close(pArray[i][0]);

		    int sent_bytes;

		    if((sent_bytes = write(pArray[i][1], &count, sizeof(count))) == -1){ // send the count to parent and check if pipe failed

		      printf("Pipe write failed.\n");
		      exit(1);

		    }; // if

		    close(pArray[i][1]);
		    return 0;
		    
		  } // if

		} // for


		int status, pids[nChildProc], finished_proc = 0;


		int k, l;
		

		
	        ////////////////////// PARENT -- all this happens after all the children terminate
		//while(waitpid((pid_t)-1, &status, 0) != -1) {
		//printf("beginning of parent\n");////////

		
		//for(k = 0; k < nChildProc; k++){ // loop through the child pid's and check status

		//printf("inside first for loop\n");//////
		//printf("value of waitpid: %d\n", waitpid((pid_t)pidArray[k], &status, 0/*WIFSIGNALED(status)*/)); ///////

		//if(waitpid((pid_t)pidArray[k], &status, 0/*WIFSIGNALED(status)*/) != -1){ // child crash

		    
		//printf("inside if(child crashed)\n");/////////

		//kill(pidArray[k], 2);
		//k = 0; // restart the waiting loop
		
		//for(l = 0; l < nChildProc; l++){ // loop through pArray to find the right pipe that is still open using the pid of the child
		
		//printf("inside loop to find the right pipe of crash\n");////////
		
		/*if(pArray[l][2] == pid){ // Found the crashed process in pArray, create new child process
		  
		  
		  pid = fork();
		  pArray[l][2] = pid;
		  
		  
		  if(pid < 0){
		  
		  printf("Fork faild.\n");
		  exit(1);
		  
		  } // if
		  
		  else if(pid == 0){ // CHILD
		  
		  //Do child stuff
		  fp = fopen(argv[1], "r");
		  
		  // Edge case: this makes sure the read size for the last child is correct
		  if((l+1) == nChildProc && ((int) fsize % (int) delegated_size) != 0){
		  count = word_count(fp, delegated_size * l, ((int) fsize % (int) delegated_size));
		  }else{
		  count = word_count(fp, delegated_size * l, delegated_size);
		  } // else
		  
		  fclose(fp);
		  
		  close(pArray[l][0]);
		  
		  int sent_bytes;
		  
		  if((sent_bytes = write(pArray[l][1], &count, sizeof(count))) == -1){ // send the count to parent and check if pipe failed
		  
		  printf("Pipe write failed.\n");
		  exit(1);
		  
		  }; // if
		  
		  close(pArray[l][1]);
		  return 0;
		  
		  } // if
		  
		  
		  
		  
		  
		  
		  
		  
		  
		  
		  
		  
		  printf("pid: %d\n", pid);/////
		  printf("pArray[%d][2]: %d\n", l, pArray[l][2]);/////
		  
		  }else{
		  
		  printf("pid: %d\n", pid);/////
		  printf("pArray[%d][2]: %d\n", l, pArray[l][2]);//////
		  printf("did not find the crashed process with the pid\n");/////
		  
		  }*/ // else
		
		//} // for
		
		//} // if
		
		//} // for
		
		
		/*while(finished_proc < nChildProc){
		//pids += pid;
		
		if(waitpid((pid_t)-1, &status, 0) != -1){ // child gracefully terminates
		
		finished_proc++; // track how many children have successfully returned
		
		} // if
		
		else if(waitpid((pid_t)-1, &status, 0) == -1){ // child crashes
		
		
		//restart the process that failed
		
		} // else if
		
		
		if(WIFSIGNALED(status)){
		printf("%d\n", pid);
		int exit_status = WEXITSTATUS(status);
		printf("Exit code: %d\n", WTERMSIG(status));
		} // if
		};*/ // while
		
		int value, index;
		
		while((value = waitpid(-1, &status, 0)) != -1){

		  if(status == 134){ // create new child and re-do the incomplete work
		    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    int crashed_pid = value; 
		    
		    for(i = 0; i < nChildProc; i++){
		      
		      if(pidArray[i] == crashed_pid){

			index = i;
			
		      } // if

		    } // for
		    
		    pidArray[index] = fork();

		    if(pidArray[index] < 0){
		      
		      printf("Fork faild.\n");
		      exit(1);
		      
		    } // if

		    else if(pidArray[index] == 0){ // CHILD
		      
		      //Do child stuff
		      fp = fopen(argv[1], "r");
		      
		      // Edge case: this makes sure the read size for the last child is correct
		      if((index+1) == nChildProc && ((int) fsize % (int) delegated_size) != 0){
			count = word_count(fp, delegated_size * index, ((int) fsize % (int) delegated_size));
		      }else{
			count = word_count(fp, delegated_size * index, delegated_size);
		      } // else
		      
		      fclose(fp);
		      
		      close(pArray[index][0]);
		      
		      int sent_bytes;

		      if((sent_bytes = write(pArray[index][1], &count, sizeof(count))) == -1){ // send the count to parent and check if pipe failed
			
			printf("Pipe write failed.\n");
			exit(1);
			
		      }; // if
		      
		      close(pArray[index][1]);
		      return 0;
		      
		    } // if
		    /////////////////////////////////////////////////////////////////////////////////////////////////////////		    
		  } // if
		  		  
		};
		
		int read_bytes;

		for(j = 0; j < nChildProc; j++){
		  
		  if((read_bytes = read(pArray[j][0], &buf, sizeof(buf))) == -1){
		    
		    printf("Pipe read failed\n");
		    exit(1);

		  }; // if
		  
		  if(read_bytes){

		    printf("Parent process %d read %d bytes from child %d (%'d lines, %'d words, %'d chars)\n", getpid(), read_bytes, j+1, buf.linecount, buf.wordcount, buf.charcount);

		    count.linecount += buf.linecount;
		    count.wordcount += buf.wordcount;
		    count.charcount += buf.charcount;

		  } // if

		  close(pArray[j][0]);
		  close(pArray[j][1]);

		} // for
		
		

		clock_gettime(CLOCK_REALTIME, &end);
		long seconds = end.tv_sec - begin.tv_sec;
		long nanoseconds = end.tv_nsec - begin.tv_nsec;
		double elapsed = seconds + nanoseconds*1e-9;

		printf("\n========= %s =========\n", argv[1]);
		printf("Total Lines : %d \n", count.linecount);
		printf("Total Words : %d \n", count.wordcount);
		printf("Total Characters : %d \n", count.charcount);
		printf("======== Took %.3f seconds ========\n", elapsed);

		return(0);
}

