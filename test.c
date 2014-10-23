#define _XOPEN_SOURCE 600
#include <X11/Xlibint.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
 
void main() {
  Display *dpl = XOpenDisplay(":0");
  if(XineramaIsActive(dpl)) {
    printf("Xinerama is active\n");
     
    int screens;
    XineramaScreenInfo* s = XineramaQueryScreens(dpl, &screens);
    printf("%d screens\n", screens);
    int j;
    for(j=0; j<screens; j++) {
      printf("%d: %hd,%hd %hdx%hd\n", j, s[j].x_org, s[j].y_org, s[j].width, s[j].height);
    }
    XFree(s);
  }
  else {
    printf("Xinerama is not active\n");
  }
   
   
XCloseDisplay(dpl);
}
