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

#include "CCLongPressGestureRecognizer.h"

USING_NS_CC;

bool CCLongPressGestureRecognizer::init()
{
    isRecognizing = false;
    isMoving = false;
    currTouch = NULL;
    currEvent = NULL;
    setMinimumDistanceForMove(kLongPressMinDistance);
//     setMinimumDistanceForMove(-1.f);

    setMinimumPressDuration(kLongPressMinDuration);

    return true;
}

CCLongPressGestureRecognizer::~CCLongPressGestureRecognizer()
{

}

void CCLongPressGestureRecognizer::timerDidEnd(float dt)
{
    CCLongPress * longPress = CCLongPress::create();
    longPress->location = currLocation;

    gestureRecognized(longPress);
    CCTime::gettimeofdayCocos2d(&startTime, NULL);

    isMoving = true;

    if (cancelsTouchesInView) stopTouchesPropagation(createSetWithTouch(currTouch), currEvent); //cancel touch over other views

    stopGestureRecognition();
}

bool CCLongPressGestureRecognizer::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
    if (isRecognizing) {
        stopGestureRecognition();
        return false;
    }

    isMoving = false;

    currLocation = pTouch->getLocation();
    if (!isPositionBetweenBounds(currLocation)) return false;

    currEvent = pEvent;
    currTouch = pTouch;

    schedule(schedule_selector(CCLongPressGestureRecognizer::timerDidEnd), minimumPressDuration);

    isRecognizing = true;
    return true;
}

void CCLongPressGestureRecognizer::ccTouchMoved(cocos2d::CCTouch * pTouch, cocos2d::CCEvent * pEvent)
{
    if (!isRecognizing && !isMoving) return;
    if (minimumDistanceForMove > 0)
    {


        CCLOG("TOUCH MOVE  minDistance");
        float distanceX = currLocation.x - pTouch->getLocation().x;
        float distanceY = currLocation.y - pTouch->getLocation().y;
        float distance = distanceBetweenPoints(currLocation, pTouch->getLocation());

        struct cc_timeval currentTime;
        CCTime::gettimeofdayCocos2d(&currentTime, NULL);

        double duration = CCTime::timersubCocos2d(&startTime, &currentTime);//duration in milliseconds
        if (distance >= minimumDistanceForMove && duration >= minimumPressDuration)
        {
            currLocation = pTouch->getLocation();
            CCLongPress * longPress = CCLongPress::create();
            longPress->location = currLocation;

            gestureMoveRecognized(longPress);
            startTime = currentTime;
            currTouch = pTouch;
        }
    }
}


void CCLongPressGestureRecognizer::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
{
    stopGestureRecognition();
    isMoving = false;
}

void CCLongPressGestureRecognizer::stopGestureRecognition()
{
    if (!isRecognizing) return;

    currTouch = NULL;
    currEvent = NULL;
    unschedule(schedule_selector(CCLongPressGestureRecognizer::timerDidEnd));
    isRecognizing = false;
}
