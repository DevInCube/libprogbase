#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <progbase/events.h>
#include <progbase/console.h>

/* custom constant event type ids*/
enum {
	KeyInputEventTypeId,
	RandomNumberEventTypeId,
	CustomEventTypeId
};

/* event handler functions prototypes */
void RandomNumberGenerator_update(EventHandler * self, Event * event);
void UpdatePrintHandler_update(EventHandler * self, Event * event);
void KeyInputHandler_update(EventHandler * self, Event * event);
void CustomHandler_handleEvent(EventHandler * self, Event * event);
void Timer_handleEvent(EventHandler * self, Event * event);
void HitCounter_handleEvent(EventHandler * self, Event * event);

typedef struct Timer Timer;
struct Timer {
	int id;
	int timeCounter;
};

int main(void) {
	// startup event system
	EventSystem_init();

	// add event handlers
	EventSystem_addHandler(EventHandler_new(NULL, NULL, UpdatePrintHandler_update));
	EventSystem_addHandler(EventHandler_new(NULL, NULL, KeyInputHandler_update));
	EventSystem_addHandler(EventHandler_new(NULL, NULL, RandomNumberGenerator_update));
	int * spaceHitCounter = malloc(sizeof(int));
	*spaceHitCounter = 0;
	EventSystem_addHandler(EventHandler_new(spaceHitCounter, free, HitCounter_handleEvent));
	Timer * timer = malloc(sizeof(Timer));
	timer->id = 0;
	timer->timeCounter = 100;
	EventSystem_addHandler(EventHandler_new(timer, free, Timer_handleEvent));
	EventSystem_addHandler(EventHandler_new(NULL, NULL, CustomHandler_handleEvent));

	// start infinite event loop
	EventSystem_loop();
	// cleanup event system
	EventSystem_cleanup();
	return 0;
}

/* event handlers functions implementations */

void UpdatePrintHandler_update(EventHandler * self, Event * event) {
	switch (event->type) {
		case StartEventTypeId: {
			puts("");
			puts("<<<START!>>>");
			puts("");
			break;
		}
		case UpdateEventTypeId: {
			putchar('.');
			break;
		}
		case ExitEventTypeId: {
			puts("");
			puts("<<<EXIT!>>>");
			puts("");
			break;
		}
	}
}

void KeyInputHandler_update(EventHandler * self, Event * event) {
	if (Console_isKeyDown()) {  // non-blocking key input check
		char * keyCode = malloc(sizeof(char));
		*keyCode = getchar();
		EventSystem_emit(Event_new(self, KeyInputEventTypeId, keyCode, free));
	}
}

void RandomNumberGenerator_update(EventHandler * self, Event * event) {
	switch (event->type) {
		case StartEventTypeId: {
			srand(time(0));
			break;
		}
		case UpdateEventTypeId: {
			if (rand() % 33 == 0) {
				int * number = malloc(sizeof(int));
				*number = rand() % 200 - 100;
				EventSystem_emit(Event_new(self, RandomNumberEventTypeId, number, free));
			}
			break;
		}
	}
}

void HitCounter_handleEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case CustomEventTypeId: {
			int * counterPtr = (int *)self->data;
			(*counterPtr)++;
			printf(">>> Custom event! Counter: %i\n", *counterPtr);
			break;
		}
	}
}

void CustomHandler_handleEvent(EventHandler * self, Event * event) {
	switch (event->type) {
		case RandomNumberEventTypeId: {
			int * number = (int *)event->data;
			printf("Random number %i\n", *number);
			break;
		}
		case KeyInputEventTypeId: {
			char * keyCodePtr = (char *)event->data;
			char keyCode = *keyCodePtr;
			if (keyCode == ' ') {
				EventSystem_emit(Event_new(self, CustomEventTypeId, NULL, NULL));
			}
			if (keyCode == 'a') {
				Timer * timer = malloc(sizeof(Timer));
				timer->id = rand() % 100;
				timer->timeCounter = 50;
				EventSystem_addHandler(EventHandler_new(timer, free, Timer_handleEvent));
			}
			if (keyCode == 'q') {
				EventSystem_exit();
			}
			printf("Key pressed `%c` [%i]\n", keyCode, keyCode);
			break;
		}
	} 
}

void Timer_handleEvent(EventHandler * self, Event * event) {
	switch(event->type) {
		case UpdateEventTypeId: {
			Timer * timer = (Timer *)self->data;
			double elapsedMillis = *(double *)event->data;
			timer->timeCounter -= 1;
			if (timer->timeCounter % 10 == 0) {
				printf("\nTimer #%i: %i {%.1lf}\n", timer->id, timer->timeCounter, elapsedMillis); 
			}
			if (timer->timeCounter == 0) {
				printf("\nTimer #%i: COMPLETED!\n", timer->id); 
				EventSystem_removeHandler(self);
			}
			break;
		}
	}
}