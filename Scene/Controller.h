#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Callbacks.h"

class Controller: public Callbacks
{
public:
    Controller(Model& m,View& v);
    ~Controller();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char* description);

    void onMouseClick(int button, int action, int mods);        // Callback for mouse button events
    void onMouseMove(double x, double y);                       // Callback for mouse movement events
    bool isDragging = false;                                    // Indicates if the mouse is being dragged
    glm::vec3 lastTrackballPos;                                 // Last position of the trackball

private:
    void initScenegraph();
    View view;
    Model model;
    void createDrone();
    float time;
    

};

#endif