
#include <SDL/SDL.h>
#import <Cocoa/Cocoa.h>

static int sdlMainExitStatus;
static int ARGC;
static char **ARGV;

@interface ApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSBundle *bundle = [NSBundle mainBundle];
	if (bundle != nil) {
		char **argv = (char **)calloc(1, sizeof(char*));
		int size = [[bundle resourcePath] length] + 1;
		argv[0] = calloc(size, sizeof(char));
		strcpy(argv[0], [[bundle resourcePath] UTF8String]);
		sdlMainExitStatus = SDL_main(1, argv);
		free(argv[0]);
		free(argv);
	} else {
		sdlMainExitStatus = SDL_main(ARGC, ARGV);
	}
    [NSApp stop:nil];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);	

	return NO;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return NO;
}

@end

static int ApplicationMain(int argc, char *argv[])
{
	ARGC = argc; ARGV = argv;

	NSAutoreleasePool *autoReleasePool = [NSAutoreleasePool new];
	ApplicationDelegate *appDelegate = [ApplicationDelegate new];

    [NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp setDelegate:appDelegate];

	/* Init Application Menus */
    NSMenu* mainMenuBar = [NSMenu new];
	[NSApp setMainMenu:mainMenuBar];

	NSMenuItem* mainMenuItem = [NSMenuItem new];
	[mainMenuBar addItem:mainMenuItem];
	  
    NSMenu* mainMenu = [NSMenu new];
	[mainMenuItem setSubmenu:mainMenu];
	  
    NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit"
												   action:@selector(terminate:)
												   keyEquivalent:@"q"];
    [mainMenu addItem:quitMenuItem];

	[quitMenuItem release];
	[mainMenu release];
	[mainMenuItem release];
	[mainMenuBar release];

	[NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
    
    [appDelegate release];
	[autoReleasePool drain];

	return sdlMainExitStatus;
}

#ifdef main
#  undef main
#endif

int main(int argc, char *argv[])
{
    return ApplicationMain(argc, argv);
}
