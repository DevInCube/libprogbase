#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <progbase/events2.h>
#include <progbase/console.h>

/* custom constant event type ids*/
enum {
	KeyInputEventTypeId,
	RandomNumberEventTypeId,
	SpaceHitEventTypeId
};

static Event * KeyInputEvent_new(EventHandler * sender, char keyCode);
static char KeyInputEvent_keyCode(Event * event);

static Event * RandomNumberEvent_new(EventHandler * sender);
static int RandomNumberEvent_number(Event * event);

static Event * SpaceHitEvent_new(EventHandler * sender);

// stateless handlers
void RandomNumberGenerator_onEvent(EventHandler * self, Event * event);
void Logger_onEvent(EventHandler * self, Event * event);
void KeyInput_onEvent(EventHandler * self, Event * event);
void MainEventsListener_onEvent(EventHandler * self, Event * event);

// timer
typedef struct Timer Timer;

Timer * Timer_new(int id, int count);
void Timer_free(Timer * self);
void Timer_onEvent(EventHandler * self, Event * event);

// space hit counter
int * SpaceHitCounter_new(void);
void SpaceHitCounter_free(int * self);
void SpaceHitCounter_onEvent(EventHandler * self, Event * event);

int main(void) {
	// startup event system
	EventSystem_init();

	// add stateless event handlers
	EventSystem_addHandler(EventHandler_new(NULL, Logger_onEvent));
	EventSystem_addHandler(EventHandler_new(NULL, KeyInput_onEvent));
	EventSystem_addHandler(EventHandler_new(NULL, RandomNumberGenerator_onEvent));
	EventSystem_addHandler(EventHandler_new(NULL, MainEventsListener_onEvent));
	// add stateful event handlers
	EventSystem_addHandler(EventHandler_new(
		ESObject_new(SpaceHitCounter_new(), (DestructorFunction)SpaceHitCounter_free),
		SpaceHitCounter_onEvent));
	EventSystem_addHandler(EventHandler_new(
		ESObject_new(Timer_new(0, 100), (DestructorFunction)Timer_free), 
		Timer_onEvent));

	// start infinite event loop
	EventSystem_loop();
	// cleanup event system
	EventSystem_cleanup();
	return 0;
}

/* event handlers functions implementations */

/* stateless handlers */

void Logger_onEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case StartEventTypeId: {
			puts("\n<<<START!>>>\n");
			break;
		}
		case UpdateEventTypeId: {
			putchar('.');
			break;
		}
		case ExitEventTypeId: {
			puts("\n<<<EXIT!>>>\n");
			break;
		}
	}
}

void KeyInput_onEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case UpdateEventTypeId: {
			if (Console_isKeyDown()) {  // non-blocking key input check
				EventSystem_emit(KeyInputEvent_new(self, getchar()));
			}
			break;
		}
	}
}

void RandomNumberGenerator_onEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case StartEventTypeId: {
			srand(time(0));
			break;
		}
		case UpdateEventTypeId: {
			if (rand() % 33 == 0) {
				EventSystem_emit(RandomNumberEvent_new(self));
			}
			break;
		}
	}
}

void MainEventsListener_onEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case RandomNumberEventTypeId: {
			printf("Random number %i\n", RandomNumberEvent_number(event));
			break;
		}
		case KeyInputEventTypeId: {
			char keyCode = KeyInputEvent_keyCode(event);
			if (keyCode == ' ') {
				EventSystem_emit(SpaceHitEvent_new(self));
			}
			if (keyCode == 'a') {
				EventSystem_addHandler(EventHandler_new(
					ESObject_new(Timer_new(rand() % 100, 50), (DestructorFunction)Timer_free), 
					Timer_onEvent));
			}
			if (keyCode == 'q') {
				EventSystem_exit();
			}
			printf("Key pressed `%c` [%i]\n", keyCode, keyCode);
			break;
		}
	} 
}

/* stateful handlers */
struct Timer {
	int id;
	int timeCounter;
};
Timer * Timer_new(int id, int count) {
	Timer * timer = malloc(sizeof(Timer));
	timer->id = id;
	timer->timeCounter = count;
	return timer;
}
void Timer_free(Timer * self) { free(self); }
void Timer_onEvent(EventHandler * self, Event * event) {
	switch(event->type) {
		case UpdateEventTypeId: {
			Timer * timer = (Timer *)ESObject_ref(self->state);
			timer->timeCounter -= 1;
			if (timer->timeCounter % 10 == 0) {
				printf("\nTimer #%i: %i {%.1lf}\n", 
					timer->id, 
					timer->timeCounter, 
					UpdateEvent_elapsedMillis(event)); 
			}
			if (timer->timeCounter == 0) {
				printf("\nTimer #%i: COMPLETED!\n", timer->id); 
				EventSystem_removeHandler(self);
			}
			break;
		}
	}
}

int * SpaceHitCounter_new(void) {
	int * spaceHitCounter = malloc(sizeof(int));
	*spaceHitCounter = 0;
	return spaceHitCounter;
}
void SpaceHitCounter_free(int * self) { free(self); }
void SpaceHitCounter_onEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case SpaceHitEventTypeId: {
			int * counterPtr = (int *)ESObject_ref(self->state);
			(*counterPtr)++;
			printf(">>> Custom event! Counter: %i\n", *counterPtr);
			break;
		}
	}
}

/* events functions */

static Event * KeyInputEvent_new(EventHandler * sender, char keyCode) {
	char * keyCodeData = malloc(sizeof(char));
    *keyCodeData = keyCode;
	ESObject * o = ESObject_new(keyCodeData, free);
	return Event_new(sender, KeyInputEventTypeId, o);
}
static char KeyInputEvent_keyCode(Event * event) {
	return *(char *)ESObject_ref(event->payload);
}

static Event * RandomNumberEvent_new(EventHandler * sender) {
	int * number = malloc(sizeof(int));
	*number = rand() % 200 - 100;
	ESObject * o = ESObject_new(number, free);
	return Event_new(sender, RandomNumberEventTypeId, o);
}
static int RandomNumberEvent_number(Event * event) {
	return *(int *)ESObject_ref(event->payload);
}

static Event * SpaceHitEvent_new(EventHandler * sender) {
	return Event_new(sender, SpaceHitEventTypeId, NULL);
}