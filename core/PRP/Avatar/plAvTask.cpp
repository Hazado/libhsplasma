#include "plAvTask.h"

/* plAvTask */
plAvTask::plAvTask() { }
plAvTask::~plAvTask() { }

IMPLEMENT_CREATABLE(plAvTask, kAvTask, plCreatable)

void plAvTask::read(hsStream* S, plResManager* mgr) { }
void plAvTask::write(hsStream* S, plResManager* mgr) { }
void plAvTask::IPrcWrite(pfPrcHelper* prc) { }

void plAvTask::IPrcParse(const pfPrcTag* tag, plResManager* mgr) {
    plCreatable::IPrcParse(tag, mgr);
}


/* plAvAnimTask */
plAvAnimTask::plAvAnimTask()
            : fInitialBlend(0.0f), fTargetBlend(0.0f), fFadeSpeed(0.0f),
              fSetTime(0.0f), fStart(false), fLoop(false), fAttach(false) { }

plAvAnimTask::~plAvAnimTask() { }

IMPLEMENT_CREATABLE(plAvAnimTask, kAvAnimTask, plAvTask)

void plAvAnimTask::read(hsStream* S, plResManager* mgr) {
    fAnimName = S->readSafeStr();
    fInitialBlend = S->readFloat();
    fTargetBlend = S->readFloat();
    fFadeSpeed = S->readFloat();
    fSetTime = S->readFloat();
    fStart = S->readBool();
    fLoop = S->readBool();
    fAttach = S->readBool();
}

void plAvAnimTask::write(hsStream* S, plResManager* mgr) {
    S->writeSafeStr(fAnimName);
    S->writeFloat(fInitialBlend);
    S->writeFloat(fTargetBlend);
    S->writeFloat(fFadeSpeed);
    S->writeFloat(fSetTime);
    S->writeBool(fStart);
    S->writeBool(fLoop);
    S->writeBool(fAttach);
}

void plAvAnimTask::IPrcWrite(pfPrcHelper* prc) {
    prc->startTag("AnimTaskParams");
    prc->writeParam("AnimName", fAnimName);
    prc->writeParam("InitialBlend", fInitialBlend);
    prc->writeParam("TargetBlend", fTargetBlend);
    prc->writeParam("FadeSpeed", fFadeSpeed);
    prc->writeParam("SetTime", fSetTime);
    prc->writeParam("Start", fStart);
    prc->writeParam("Loop", fLoop);
    prc->writeParam("Attach", fAttach);
    prc->endTag(true);
}

void plAvAnimTask::IPrcParse(const pfPrcTag* tag, plResManager* mgr) {
    if (tag->getName() == "AnimTaskParams") {
        fAnimName = tag->getParam("AnimName", "");
        fInitialBlend = tag->getParam("InitialBlend", "0").toFloat();
        fTargetBlend = tag->getParam("TargetBlend", "0").toFloat();
        fFadeSpeed = tag->getParam("FadeSpeed", "0").toFloat();
        fSetTime = tag->getParam("SetTime", "0").toFloat();
        fStart = tag->getParam("Start", "false").toBool();
        fLoop = tag->getParam("Loop", "false").toBool();
        fAttach = tag->getParam("Attach", "false").toBool();
    } else {
        plCreatable::IPrcParse(tag, mgr);
    }
}


/* plAvOneShotTask */
plAvOneShotTask::plAvOneShotTask() { }
plAvOneShotTask::~plAvOneShotTask() { }

IMPLEMENT_CREATABLE(plAvOneShotTask, kAvOneShotTask, plAvTask)


/* plAvOneShotLinkTask */
plAvOneShotLinkTask::plAvOneShotLinkTask() { }
plAvOneShotLinkTask::~plAvOneShotLinkTask() { }

IMPLEMENT_CREATABLE(plAvOneShotLinkTask, kAvOneShotLinkTask, plAvOneShotTask)

void plAvOneShotLinkTask::read(hsStream* S, plResManager* mgr) {
    fAnimName = S->readSafeStr();
    fMarkerName = S->readSafeStr();
}

void plAvOneShotLinkTask::write(hsStream* S, plResManager* mgr) {
    S->writeSafeStr(fAnimName);
    S->writeSafeStr(fMarkerName);
}

void plAvOneShotLinkTask::IPrcWrite(pfPrcHelper* prc) {
    prc->startTag("OneShotLinkParams");
    prc->writeParam("AnimName", fAnimName);
    prc->writeParam("MarkerName", fMarkerName);
    prc->endTag(true);
}

void plAvOneShotLinkTask::IPrcParse(const pfPrcTag* tag, plResManager* mgr) {
    if (tag->getName() == "OneShotLinkParams") {
        fAnimName = tag->getParam("AnimName", "");
        fMarkerName = tag->getParam("MarkerName", "");
    } else {
        plCreatable::IPrcParse(tag, mgr);
    }
}


/* plAvSeekTask */
plAvSeekTask::plAvSeekTask() { }
plAvSeekTask::~plAvSeekTask() { }

IMPLEMENT_CREATABLE(plAvSeekTask, kAvSeekTask, plAvTask)


/* plAvTaskBrain */
plAvTaskBrain::plAvTaskBrain() : fBrain(NULL) { }

plAvTaskBrain::~plAvTaskBrain() {
    if (fBrain != NULL)
        delete fBrain;
}

IMPLEMENT_CREATABLE(plAvTaskBrain, kAvTaskBrain, plAvTask)

void plAvTaskBrain::read(hsStream* S, plResManager* mgr) {
    setBrain(plArmatureBrain::Convert(mgr->ReadCreatable(S)));
}

void plAvTaskBrain::write(hsStream* S, plResManager* mgr) {
    mgr->WriteCreatable(S, fBrain);
}

void plAvTaskBrain::IPrcWrite(pfPrcHelper* prc) {
    if (fBrain != NULL) {
        prc->writeSimpleTag("Brain");
        fBrain->prcWrite(prc);
        prc->closeTag();
    } else {
        prc->startTag("Brain");
        prc->writeParam("NULL", true);
        prc->closeTag();
    }
}

void plAvTaskBrain::IPrcParse(const pfPrcTag* tag, plResManager* mgr) {
    if (tag->getName() == "Brain") {
        if (tag->getParam("NULL", "false").toBool())
            setBrain(NULL);
        else if (tag->hasChildren())
            setBrain(plArmatureBrain::Convert(mgr->prcParseCreatable(tag->getFirstChild())));
    } else {
        plCreatable::IPrcParse(tag, mgr);
    }
}

plArmatureBrain* plAvTaskBrain::getBrain() const { return fBrain; }

void plAvTaskBrain::setBrain(plArmatureBrain* brain) {
    if (fBrain != NULL)
        delete fBrain;
    fBrain = brain;
}


/* plAvTaskSeek */
plAvTaskSeek::plAvTaskSeek() { }
plAvTaskSeek::~plAvTaskSeek() { }

IMPLEMENT_CREATABLE(plAvTaskSeek, kAvTaskSeek, plAvTask)