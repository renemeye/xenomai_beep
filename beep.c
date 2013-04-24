#include <stdio.h>
#include <unistd.h> /* for libc5 */
#include <sys/io.h> 
#include <signal.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

#define SOUND_PORT_INTERN 0x61
#define SOUND_PORT 0x378
#define SOUND_BIT_INTERN 2
#define SOUND_BIT 6

int freq[8] = {440, 523, 587, 659, 784, 880, 988, 1046}; /*A C D E F G A B C in Hz*/
int taskHasBeenStarted = 0;
RT_TASK beep_task;

void cleanup(int blubb){
	printf("\nexit me\n");

	if(taskHasBeenStarted)
	rt_task_delete(
			&beep_task
	);
	
	_exit(0);
}

void toggleSpeaker(){
		outb(inb(SOUND_PORT)^(1<<SOUND_BIT),SOUND_PORT);
}

void makeOneBeep(int freqNr, int duration_in_seconds){
	long ticks_between_toggle = rt_timer_ns2ticks((1000000000/2)/freq[freqNr]);
	RTIME next_toggle_time = rt_timer_read();
	RTIME stop_time = rt_timer_ns2ticks(duration_in_seconds*1000000000)+next_toggle_time;

	while(next_toggle_time < stop_time){
		next_toggle_time+=ticks_between_toggle;
		rt_task_sleep_until(next_toggle_time);
		toggleSpeaker();
	}
}

void beep(){
	
	makeOneBeep(0,1);
	makeOneBeep(1,1);
	makeOneBeep(2,1);
	makeOneBeep(3,1);
	makeOneBeep(4,1);

	return;
}

int main () {
	//printf("Set signal handler\n");
	signal(SIGINT,cleanup);
	signal(SIGPIPE,cleanup);
	signal(SIGQUIT,cleanup);
	signal(SIGHUP,cleanup);
	signal(SIGTERM,cleanup);

	//printf("Get shure, that we will not beeing paged\n");
	mlockall(MCL_CURRENT|MCL_FUTURE);

#if 1
	if(ioperm(SOUND_PORT,1,1)){
		printf("NOT ALLOWED to use SoundPort\n");
	}
#endif


	rt_task_create(
		&beep_task,    // Task "object"
		"Beep Task",  // Taskname
		0, 	      		// stacksize
		99, 	      	// priority
		0             // option-flags
	);
	taskHasBeenStarted = 1;
	rt_task_start(
			&beep_task,	// Task 
			beep, 			// fuction pointer
			NULL				// Data
	);

	pause();

	rt_task_delete(
			&beep_task
	);
	
	return 0;
}

