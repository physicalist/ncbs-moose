/*
** GuiHandler.h
** 
** Made by 
** Login   <subha@pinni>
** 
** Started on  Wed Jul 18 10:38:34 2007 
// Last update Wed Jul 18 10:44:38 2007 
*/

#ifndef   	GUIHANDLER_H_
# define   	GUIHANDLER_H_

// Event handling:
// 	The shell should be interfaced through this class to post events to the GUI.
// 	Shell user should be able to:
// 		1. trigger action for every menu item
// 		2. resize the objects that can be resized
// 		3. apply the tool actions on appropriate objects
// 		4. redirect data to graph object
// 		5. display the model in GUI

// Whenever something is created in GUI, we should have an Id for that
// object to maintain the general principle of MOOSE core. But this
// will be difficult because then every qt class needs to be wrapped
// in a MOOSE element. Also the Qt classes do not have Cinfos, which
// means we may need to create another layer of wrapper over them.


class GuiHandler
{
  public:
    Id createPlot();
    
  private:
    
};



#endif 	    /* !GUIHANDLER_H_ */
