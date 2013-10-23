/****************************************************************************
Copyright (c) 2013 Artavazd Barseghyan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCSwipeGestureRecognizer.h"
#include <iostream>

USING_NS_CC;

bool CCSwipeGestureRecognizer::init()
{
    direction = 0;
    _minDistance = kSwipeMinDistance;
    _maxDuration = kSwipeMaxDuration;

    return true;
}

CCSwipeGestureRecognizer::~CCSwipeGestureRecognizer()
{

}

bool CCSwipeGestureRecognizer::checkSwipeDirection(CCPoint p1, CCPoint p2, int & dir, int & dir_m)
{
    bool right = p2.x-p1.x>=_minDistance;
    bool left = p1.x-p2.x>=_minDistance;
    bool down = p1.y-p2.y>=_minDistance;
    bool up = p2.y-p1.y>=_minDistance;

    dir_m = 0;

#if defined(COCOS2D_DEBUG)
    CCLOG("CHECK SWIPE r %i l %i d %i u %i dire %i ", right, left, down, up, direction);
#endif

    if (left && (direction & kSwipeGestureRecognizerDirectionLeft))
        dir_m |= kSwipeGestureRecognizerDirectionLeft;
    else if (right && (direction & kSwipeGestureRecognizerDirectionRight))
        dir_m |= kSwipeGestureRecognizerDirectionRight;

    if (down && (direction & kSwipeGestureRecognizerDirectionDown))
        dir_m |= kSwipeGestureRecognizerDirectionDown;
    else if (up && (direction & kSwipeGestureRecognizerDirectionUp))
        dir_m |= kSwipeGestureRecognizerDirectionUp;

#if defined(COCOS2D_DEBUG)
    CCLOG("dir_m %i left %i right %i down %i up %i", dir_m, (dir_m & kSwipeGestureRecognizerDirectionLeft) == kSwipeGestureRecognizerDirectionLeft,
          (dir_m & kSwipeGestureRecognizerDirectionRight) == kSwipeGestureRecognizerDirectionRight,
          (dir_m & kSwipeGestureRecognizerDirectionDown) == kSwipeGestureRecognizerDirectionDown,
          (dir_m & kSwipeGestureRecognizerDirectionUp) == kSwipeGestureRecognizerDirectionUp);
#endif

    if (dir_m != 0)
        return true;

    return false;

    if (right && !down && !up) {
        if ((direction & kSwipeGestureRecognizerDirectionRight) == kSwipeGestureRecognizerDirectionRight) {
            dir = kSwipeGestureRecognizerDirectionRight;
            return true;
        }
    }
    else if (left  && !down && !up) {
        if ((direction & kSwipeGestureRecognizerDirectionLeft) == kSwipeGestureRecognizerDirectionLeft) {
            dir = kSwipeGestureRecognizerDirectionLeft;
            return true;
        }
    }
    else if (up  && !left && !right) {
        if ((direction & kSwipeGestureRecognizerDirectionUp) == kSwipeGestureRecognizerDirectionUp) {
            dir = kSwipeGestureRecognizerDirectionUp;
            return true;
        }
    }
    else if (down && !left && !right) {
        if ((direction & kSwipeGestureRecognizerDirectionDown) == kSwipeGestureRecognizerDirectionDown) {
            dir = kSwipeGestureRecognizerDirectionDown;
            return true;
        }
    }

    return false;
}

bool CCSwipeGestureRecognizer::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
    if (isRecognizing) {
        isRecognizing = false;
        return false;
    }

    initialPosition = pTouch->getLocation();
    if (!isPositionBetweenBounds(initialPosition)) return false;

    CCTime::gettimeofdayCocos2d(&startTime, NULL);

    isRecognizing = true;
    return true;
}

void CCSwipeGestureRecognizer::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
{
    CCPoint finalPosition = pTouch->getLocation();
    if (!isPositionBetweenBounds(finalPosition)) {
        isRecognizing = false;
        return;
    }

    //distance between initial and final point of touch
    float distance = distanceBetweenPoints(initialPosition, finalPosition);

    struct cc_timeval currentTime;
    CCTime::gettimeofdayCocos2d(&currentTime, NULL);

    double duration = CCTime::timersubCocos2d(&startTime, &currentTime); //duration in milliseconds

    //check that minimum distance has been reached
    //check that maximum duration hasn't been reached
    //check if the direction of the swipe is correct
    int dir = 0;
    int dir_m = 0;
    if (distance>=_minDistance && duration<=_maxDuration && checkSwipeDirection(initialPosition,finalPosition,dir, dir_m)) {
        CCSwipe * swipe = CCSwipe::create();
        swipe->direction = (CCSwipeGestureRecognizerDirection)dir;
        swipe->bit_directions = dir_m;
        swipe->location = initialPosition;
        swipe->finalPosition = finalPosition;

        gestureRecognized(swipe);
        if (cancelsTouchesInView) stopTouchesPropagation(createSetWithTouch(pTouch), pEvent); //cancel touch over other views
    }

    isRecognizing = false;
}
