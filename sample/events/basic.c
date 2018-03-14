#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <progbase/events.h>
#include <progbase/console.h>

/* custom constant event type ids*/
enum {
	CustomEventTypeId
};

/* event handler functions prototypes */
void CustomHandler_update(EventHandler * self, Event * event);

int main(void) {
	// startup event system
	EventSystem_init();

	// add event handlers
	EventSystem_addHandler(EventHandler_new(NULL, NULL, CustomHandler_update));

	// start infinite event loop
	EventSystem_loop();
	// cleanup event system
	EventSystem_cleanup();
	return 0;
}

/* event handlers functions implementations */

void CustomHandler_update(EventHandler * self, Event * event) {
	// @todo implement
	puts("Got some event!");
}
