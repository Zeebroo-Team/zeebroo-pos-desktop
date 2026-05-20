#include "core/PosBeep.h"

#include <QCoreApplication>
#include <QSoundEffect>
#include <QUrl>

namespace pos {

static QSoundEffect *beepEffect()
{
    static QSoundEffect *effect = nullptr;
    if (effect == nullptr) {
        effect = new QSoundEffect(QCoreApplication::instance());
        effect->setSource(QUrl(QStringLiteral("qrc:/sounds/beep.wav")));
        effect->setVolume(0.85f);
        effect->setLoopCount(1);
    }
    return effect;
}

void PosBeep::preload()
{
    beepEffect();
}

void PosBeep::play()
{
    QSoundEffect *effect = beepEffect();
    if (effect->status() == QSoundEffect::Error) {
        return;
    }

    effect->stop();
    effect->play();
}

} // namespace pos
