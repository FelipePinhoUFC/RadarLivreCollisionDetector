#include <iostream>

#include <rlrepository.h>
#include <rlremoterepository.h>
#include <rlsimulatorrepository.h>
#include <rlcollisiondetector.h>

class AlertListener: public CollisionAlertListener {
    public:
        void onAlert(CollisionAlert a) {
            // cout << endl << a.toString() << endl;
        }
};

int main(int argc, char * argv[]) {

    int airplaneCount = 3000;
    unsigned int iterations = 10;
    bool useThreads = 1;
    bool useAreaDivision = 0;

    if(argc >= 5) {
        airplaneCount = atoi(argv[1]);
        iterations = atoi(argv[2]);
        useThreads = atoi(argv[3]);
        useAreaDivision = atoi(argv[4]);
    }


    // Repository * r = new SimulatorRepository(airplaneCount);
    Repository * r = new RemoteRepository();

    cout << "#TEST" << endl
         << "#AIRPLANES: " << r->aircrafts.size() << endl
         << "#THREADS: " << (useThreads? "TRUE": "FALSE") << endl
         << "#USE AREA DIVISION: " << (useAreaDivision? "TRUE": "FALSE") << endl
         << "#ITERATIONS: " << iterations << endl << endl;

    AlertListener * listener= new AlertListener();
    CollisionDetector * detector = new CollisionDetector(r, listener, useThreads, useAreaDivision);
    //detector->showSystemStatus();

    for(unsigned int i = 0; i < iterations; i++) {
        cout << "TEST " << i << endl;
        detector->findIminentCollisions(false);
        r->reloadAircrafts();
        cout << endl;
    }

}
