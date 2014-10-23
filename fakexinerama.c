#ifdef notdef
set -x
exec cc -O2 -std=c99 -pedantic -Wall $0 -fPIC -o libXinerama.so -shared
#endif
#define _XOPEN_SOURCE 600
#include <X11/Xlibint.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
 
typedef XineramaScreenInfo* (queryfn)(Display*, int*);
typedef Status (versionfn)(Display*, int*, int*);
typedef Bool (extensionfn)(Display*, int*, int*);
typedef Bool (activefn)(Display*);
 
static void* libxinerama;
static queryfn* queryscreens;
static extensionfn* queryextension;
static versionfn* queryversion;
static activefn* isactive;
 
static void init() {
  libxinerama = dlopen("/usr/lib64/REALlibXinerama.so.1", RTLD_GLOBAL | RTLD_LAZY);
  if(!libxinerama)
  abort();
  isactive = (activefn*) (uintptr_t)dlsym(libxinerama, "XineramaIsActive");
  queryversion = (versionfn*) (uintptr_t)dlsym(libxinerama, "XineramaQueryVersion");
  queryextension = (extensionfn*) (uintptr_t)dlsym(libxinerama, "XineramaQueryExtension");
  queryscreens = (queryfn*) (uintptr_t)dlsym(libxinerama, "XineramaQueryScreens");
}
 
Bool XineramaQueryExtension(Display *dpy, int *event_base, int *error_base) {
  init();
  return queryextension(dpy, event_base, error_base);
}
 
Status XineramaQueryVersion(Display *dpy, int *major, int *minor) {
  init();
  return queryversion(dpy, major, minor);
}
 
Bool XineramaIsActive(Display *dpy) {
  init();
  return isactive(dpy);
}
 
XineramaScreenInfo* XineramaQueryScreens(Display *dpy, int *number)
{
  XineramaScreenInfo *ret, *work;
  int i=0; 
  int y=0;
  int numberOfScreens=0;
  init();
  
  ret = queryscreens(dpy, number);
  /*
   * We use screen_number=99 to mean that we have already claimed
   * that screen as being the other half of another
   * */
  
  while(i<*number){///Cycle through Actual Xinerama Screens looking for a spilt 3840x2160 ones
    if(ret[i].width==1920 && ret[i].height==2160){//We have half a screen, yay!
      if(ret[i].screen_number==99){//This half screen has already been claimed as the second half of another, so skip it.
	i++;
	continue;
      }
      y=0;
      int foundOtherHalf=0;
      int otherHalfLeftOfThis=0;
      while(y<*number){//Lets Look for that other half!
	if(ret[y].screen_number!=99 && ret[y].width==1920 && ret[y].height==2160){// is a half
	  if(ret[y].y_org==ret[i].y_org && (ret[i].width+ret[i].x_org)==ret[y].x_org){
	    //Is the other half we are looking for, and it is to the Right of the first
	    if(ret[i].screen_number>ret[y].screen_number)
	      ret[i].screen_number=ret[y].screen_number;
	    ret[y].screen_number=99;
	    foundOtherHalf=1;
	    break;
	  }else if(ret[y].y_org==ret[i].y_org && (ret[y].width+ret[y].x_org)==ret[i].x_org){
	    //Is the other half we are looking for, and it is to the Left of the first
	    if(ret[i].screen_number>ret[y].screen_number)
	      ret[i].screen_number=ret[y].screen_number;
	    ret[y].screen_number=99;
	    foundOtherHalf=1;
	    otherHalfLeftOfThis=1;
	    break;
	  }
	}
	y++;
      }
      if(foundOtherHalf){//If we have another half then
	ret[i].width=3840;//Set Width to Full
	if(otherHalfLeftOfThis)
	  ret[i].x_org-=1920;
	numberOfScreens++;
      }else{//We didn't find the other half, so we will just continue as if this half was a screen in its own right
	
	numberOfScreens++;
      }
    }else{
      numberOfScreens++;
    }
    i++;
  }
  work = Xmalloc(numberOfScreens * sizeof(XineramaScreenInfo));
  i=0;
  y=0;
  while(i<*number){
    if(ret[i].screen_number!=99){//if it is a proper screen
      work[y].screen_number=y;
      work[y].x_org=ret[i].x_org;
      work[y].y_org=ret[i].y_org;
      work[y].width=ret[i].width;
      work[y].height=ret[i].height;
      y++;
    }
    i++;
  }
  XFree(ret);
  ret=work;
  *number = numberOfScreens;
  
  return ret;
}
