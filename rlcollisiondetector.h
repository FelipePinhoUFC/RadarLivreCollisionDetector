#ifndef RLCOLLISIONDETECTOR_H
#define RLCOLLISIONDETECTOR_H

#include <iostream>
#include <functional>
#include <vector>
#include <list>
#include <bitset>
#include <thread>
#include <mutex>
#include <climits>

#include <rlmath.h>
#include <rlutil.h>
#include <rlcollisionalert.h>
#include <rlrepository.h>
#include <rlcollisionalertlistener.h>
#include <rlcombinator.h>

using namespace std;

class CollisionDetector : public PairCombinatorListener<Aircraft*> {

    public:

        CollisionDetector(Repository * repository, CollisionAlertListener * listener = nullptr, bool useThreads = true, bool useAreaDivision = true, int propagationInterations = 1)
            : __repository(repository), __listener(listener) {
            __propagationInterations = propagationInterations;
            __useThreads = useThreads;
            __useAreaDivision = useAreaDivision;
            __systemStatus = false;
        }

        void showSystemStatus() {
            __systemStatus = true;
        }

        void findIminentCollisions(bool async=true) {

            __currentAsyncSearch = thread(&CollisionDetector::__findCollisions, this);

            if(!async)
                __currentAsyncSearch.join();

        }

        bool isCombination(Aircraft *a, Aircraft *b) {

            // Return true if a is in a area near b
            return a->isNear(b);
        }

        void onCombine(PairCombination<Aircraft*> * c) {

            __currentStatusSearchMutex.lock();
                __stepCount++;
            __currentStatusSearchMutex.unlock();

            Aircraft * a = c->a->getCopy();
            Aircraft * b = c->b->getCopy();

            if(!a->isValidToColisionDetection() || !b->isValidToColisionDetection())
                return;

            a->setFirstPropagationRelativeToMyself();
            b->setFirstPropagationRelativeTo(a);

            if(a->hasPazConflict(b, __propagationInterations * 1000)) {
                CollisionAlert * alertCAZ = nullptr;
                CollisionAlert * alertPAZ = nullptr;
                CollisionAlert * alert = nullptr;

                for(int i = 0; i < __propagationInterations; i++) {
                    a->propagateTrajectory(1000);
                    b->propagateTrajectory(1000);

                    bool cazConflict = a->hasCazConflict(b);
                    bool pazConflict = a->hasPazConflict(b);

                    if(cazConflict && alertCAZ == nullptr) {
                        alertCAZ = new CollisionAlert();
                        alertCAZ->type = CollisionAlert::CAZ_CONFLICT;
                        alertCAZ->timestamp = Util::getSystemTimestamp();
                        alertCAZ->timestampOfColision = Util::getSystemTimestamp(i * 1000);
                        alertCAZ->own = a->getCopy();
                        alertCAZ->target = b->getCopy();
                        alertCAZ->aircraftsDistance = a->getLastKnownInfo()->globalPosition.getDistanceTo(b->getLastKnownInfo()->globalPosition);
                        break;

                    } else if(pazConflict && alertPAZ == nullptr) {
                        alertPAZ = new CollisionAlert();
                        alertPAZ->type = CollisionAlert::PAZ_CONFLICT;
                        alertPAZ->timestamp = Util::getSystemTimestamp();
                        alertPAZ->timestampOfColision = Util::getSystemTimestamp(i * 1000);
                        alertPAZ->own = a->getCopy();
                        alertPAZ->target = b->getCopy();
                        alertPAZ->aircraftsDistance = a->getLastKnownInfo()->globalPosition.getDistanceTo(b->getLastKnownInfo()->globalPosition);

                    }
                }

                if(alertCAZ != nullptr) {
                    alert = alertCAZ;
                } else if(alertPAZ != nullptr) {
                    alert = alertPAZ;
                }

                if(__listener != nullptr && alert != nullptr) {
                    __listener->onAlert(*alert);
                }

                if(alertCAZ != nullptr)
                    delete alertCAZ;
                if(alertPAZ != nullptr)
                    delete alertPAZ;
            }

            delete a;
            delete b;
        }

    private:

        thread __currentAsyncSearch;
        thread __currentStatusSearch;
        recursive_mutex __currentStatusSearchMutex;
        Repository * __repository;
        CollisionAlertListener * __listener;
        PairCombinator<Aircraft*> * __currentCombinator;

        int __propagationInterations;
        long __expectedCombinations;
        long __stepCount;
        bool __running;
        bool __useThreads;
        bool __useAreaDivision;
        bool __systemStatus;

        void __findCollisions() {

            // To get the algorithm timelapse
            long timestamp = Util::getSystemTimestamp();

            __stepCount = 0;
            __expectedCombinations = __repository->aircrafts.size() * (__repository->aircrafts.size() - 1) / Math::factorial(2);

            list<Aircraft*> aircrafts = __repository->getAircraftsCopy();
            __currentCombinator = new PairCombinator<Aircraft*>(aircrafts, this, __useThreads, __useAreaDivision);

            __running = true;
            if(__systemStatus) __currentStatusSearch = thread(&CollisionDetector::__showStatusThread, this);

            __currentCombinator->start(false /* Synchronous execution */);

            __running = false;
            if(__systemStatus) __currentStatusSearch.join();

            delete __currentCombinator;


            long elapsed = Util::getSystemTimestamp() - timestamp;

            // Show Overal Info
            __showSystemStatus(elapsed);
        }

        void __showSystemStatus(long elapsed=0) {
            long elapsedS = elapsed / 1000;
            long elapsedM = elapsed % 1000;
            // cout << "\nIterations: " << __stepCount << ", Elapsed time: " << elapsedS << "." << elapsedM << endl;
            cout << "ELAPSED TIME: " << elapsedS << "." << elapsedM << " seconds" << endl;

            Util::showMemoryUsage();
        }

        void __showStatusThread() {

            while(__running || __stepCount < __expectedCombinations - __currentCombinator->skippedCombinations) {
                __showStatus();
            }
            __showStatus();
            cout << endl;

        }

        void __showStatus() {
            usleep(10 * 1000);
            cout << __currentCombinator->toString() << " [";
            long x = __stepCount * 10 / (__expectedCombinations - __currentCombinator->skippedCombinations);
            for(unsigned int i = 0; i < 10; i++)
                cout << (i < x? "#": i == x? ">": " ");
            cout << "] " << __stepCount << " of " << __expectedCombinations - __currentCombinator->skippedCombinations << " : "
                 << __currentCombinator->skippedCombinations
                 << "          \r";
        }

};

#endif // RLCOLLISIONDETECTOR_H
