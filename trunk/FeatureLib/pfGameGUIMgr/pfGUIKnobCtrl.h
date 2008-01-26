#ifndef _PFGUIKNOBCTRL_H
#define _PFGUIKNOBCTRL_H

#include "pfGUIValueCtrl.h"
#include "CoreLib/hsGeometry3.h"

DllClass pfGUIKnobCtrl : public pfGUIValueCtrl {
public:
    enum KnobFlags {
        kReverseValues = kDerivedFlagsStart, kLeftRightOrientation,
        kMapToScreenRange, kTriggerOnlyOnMouseUp, kMapToAnimationRange
    };

protected:
    hsTArray<plKey> fAnimationKeys;
    plString fAnimName;
    hsPoint3 fDragStart;
    float fDragValue;
    bool fDragging;
    hsPoint3 fAnimStartPos, fAnimEndPos;
    float fDragRangeMin, fDragRangeMax, fAnimBegin, fAnimEnd;
    bool fAnimTimesCalced;

public:
    pfGUIKnobCtrl();
    virtual ~pfGUIKnobCtrl();

    DECLARE_CREATABLE(pfGUIKnobCtrl)

    virtual void read(hsStream* S, plResManager* mgr);
    virtual void write(hsStream* S, plResManager* mgr);
    virtual void prcWrite(pfPrcHelper* prc);
};

#endif