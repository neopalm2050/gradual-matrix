#include <GL/freeglut.h>
#include <Eigen/Dense>
#include <cmath>
#include <deque>

struct Point {
    double x;
    double y;
};

struct Trace {
    Eigen::Vector2d front;
    int timeToDestroy;
    std::deque<Point> points;
};


Eigen::Matrix2d logfib;
int oldTimeSinceStart = 0;
std::deque<Trace> lines;
int pointCountDown = -1;

int msPerPoint = 10;
int particleLifespan = 8000;
int msPerFrame = 0;
double scale = 200;
double taper_speed = 0.99;
int max_trail_length = 200;


void timer(int arg) {
    glutPostRedisplay();
    glutTimerFunc(msPerFrame, timer, 0);
}

void display() {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    int dt = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
    
    // X-gridlines
    for (int i=1; i < windowHeight/scale; ++i) {
        glBegin(GL_LINES);
            glColor3d(0.4, 0.4, 0.4); // Grey
            glVertex2d(-1.0, i * scale / windowHeight);    // x, y
            glVertex2d( 1.0, i * scale / windowHeight);
        glEnd();

        glBegin(GL_LINES);
            glColor3d(0.4, 0.4, 0.4); // Grey
            glVertex2d(-1.0, -i * scale / windowHeight);    // x, y
            glVertex2d( 1.0, -i * scale / windowHeight);
        glEnd();
    }

    // Y-gridlines
    for (int i=1; i < windowWidth/scale; ++i) {
        glBegin(GL_LINES);
            glColor3d(0.4, 0.4, 0.4); // Grey
            glVertex2d(i * scale / windowWidth, -1.0);    // x, y
            glVertex2d(i * scale / windowWidth,  1.0);
        glEnd();

        glBegin(GL_LINES);
            glColor3d(0.4, 0.4, 0.4); // Grey
            glVertex2d(-i * scale / windowWidth, -1.0);    // x, y
            glVertex2d(-i * scale / windowWidth,  1.0);
        glEnd();
    }

    // X-axis
    glBegin(GL_LINES);
        glColor3d(1.0, 1.0, 1.0); // White
        glVertex2d(-1.0, 0.0);    // x, y
        glVertex2d( 1.0, 0.0);
    glEnd();

    // Y-axis
    glBegin(GL_LINES);
        glColor3d(1.0, 1.0, 1.0); // White
        glVertex2d(0.0, -1.0);    // x, y
        glVertex2d(0.0,  1.0);
    glEnd();

    while (!lines.empty() && lines[0].timeToDestroy < timeSinceStart) {
        lines.pop_front();
    }

    for (std::deque<Trace>::iterator it = lines.begin(); it != lines.end(); ++it) {
        (*it).points.push_front({(*it).front(0), (*it).front(1)});
        (*it).front += ((double) dt) / 1000 * logfib * (*it).front;
        double brightness = 1;
        for (int j = 1; j < (*it).points.size() && j < max_trail_length; ++j) {
            glBegin(GL_LINES);
                glColor3d(brightness, brightness, brightness);
                glVertex2d(scale * (*it).points[j-1].x / windowWidth, scale * (*it).points[j-1].y / windowHeight);
                glVertex2d(scale * (*it).points[j].x / windowWidth, scale * (*it).points[j].y / windowHeight);
            glEnd();
            brightness *= taper_speed;
        }
    }

    if (pointCountDown < 0) {
        pointCountDown += msPerPoint;

        Eigen::Vector2d front;
        int side = rand() * 4 / RAND_MAX;
        if (side == 0) {
            front(0) = (((double) rand() * 2) / RAND_MAX - 1) * windowWidth / scale;
            front(1) = windowHeight / scale;
        }
        
        else if (side == 1) {
            front(0) = windowWidth / scale;
            front(1) = (((double) rand() * 2) / RAND_MAX - 1) * windowHeight / scale;
        }

        else if (side == 2) {
            front(0) = (((double) rand() * 2) / RAND_MAX - 1) * windowWidth / scale;
            front(1) = -windowHeight / scale;
        }
        
        else {
            front(0) = -windowWidth / scale;
            front(1) = (((double) rand()* 2) / RAND_MAX - 1) * windowHeight / scale;
        }

        std::deque<Point> points;

        Trace new_trace = {front, timeSinceStart + particleLifespan, points};

        lines.push_back(new_trace);
    }

    pointCountDown -= dt;
 
    glFlush();  // Render now
}

int main(int argc, char** argv) {
    double PHI = ( 1+sqrt(5) ) / 2;
    double phi = ( 1-sqrt(5) ) / 2;

    Eigen::Matrix2d Ftransform;
    Ftransform <<  1, -phi,
                  -1,  PHI;
    
    Eigen::Matrix2d apply_eigenvalues;
    apply_eigenvalues << log(PHI + 1), 0,
                         0, log(phi + 1);
    
    Eigen::Matrix2d Btransform;
    Btransform << PHI, phi,
                  1  , 1  ;
    
    logfib = 1/(PHI - phi) * Btransform * apply_eigenvalues * Ftransform;
    
    glutInit(&argc, argv);                 // Initialize GLUT
    glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
    glutInitWindowSize(320, 320);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutTimerFunc(msPerFrame, timer, 0);
    glutMainLoop();           // Enter the event-processing loop
    return 0;
}