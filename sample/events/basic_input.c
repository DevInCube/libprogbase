#include <stdlib.h>
#include <stdio.h>

#include <progbase/events.h>
#include <progbase/console.h>

/* custom constant event type ids*/
enum {
	KeyInputEventTypeId
};

/* create typed event with data */
static Event * KeyInputEvent_new(EventHandler * sender, char keyCode);
/* get typed event data */
static char KeyInputEvent_keyCode(Event * event);

/* event handler functions prototypes */
void KeyInputHandler_onEvent(EventHandler * self, Event * event);
void KeyInputListener_onEvent(EventHandler * self, Event * event);

int main(void) {
	// startup event system
	EventSystem_init();

	// add event handlers
	EventSystem_addHandler(EventHandler_new(NULL, NULL, KeyInputHandler_onEvent));
	EventSystem_addHandler(EventHandler_new(NULL, NULL, KeyInputListener_onEvent));

	// start infinite event loop
	EventSystem_loop();
	// cleanup event system
	EventSystem_cleanup();
	return 0;
}


/* event handlers functions implementations */

void KeyInputHandler_onEvent(EventHandler * self, Event * event) {
	switch(event->type) {
		case UpdateEventTypeId: {
			if (Console_isKeyDown()) {  // non-blocking key input check
		        char keyCode = Console_getChar(); 
				if (keyCode == 27) {  // Escape key code
					EventSystem_exit();	
				} else {
					EventSystem_emit(KeyInputEvent_new(self, keyCode));
				}
			}
			break;
		}
	}
}

void KeyInputListener_onEvent(EventHandler * self, Event * event) {
	switch(event->type) {
		case StartEventTypeId: {
			puts("Press [Esc] to exit");
			break;
		}
		case KeyInputEventTypeId: {
			char keyCode = KeyInputEvent_keyCode(event);
			printf("\nNew Event >> Key `%c` (%i) was pressed", keyCode, keyCode);
			break;
		}
	}
}


/* KeyInputEventTypeId implementations */

static Event * KeyInputEvent_new(EventHandler * sender, char keyCode) {
	char * keyCodeData = malloc(sizeof(char));
    *keyCodeData = keyCode;
	return Event_new(sender, KeyInputEventTypeId, keyCodeData, free);
}

static char KeyInputEvent_keyCode(Event * event) {
	return *(char *)event->data;
}