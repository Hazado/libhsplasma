#ifndef _HSCOLORRGBA_H
#define _HSCOLORRGBA_H

#include "Stream/pfPrcHelper.h"
#include "Stream/pfPrcParser.h"

DllStruct hsColorRGBA {
public:
    static const hsColorRGBA kBlack;
    static const hsColorRGBA kWhite;
    static const hsColorRGBA kRed;
    static const hsColorRGBA kGreen;
    static const hsColorRGBA kBlue;
    static const hsColorRGBA kYellow;
    static const hsColorRGBA kMagenta;
    static const hsColorRGBA kCyan;
    static const hsColorRGBA kGray;
    static const hsColorRGBA kNone;

public:
    float r, g, b, a;

    hsColorRGBA(float red = 0.0f, float green = 0.0f, float blue = 0.0f,
                float alpha = 1.0f);
    hsColorRGBA(const hsColorRGBA& init);
    void set(float red, float green, float blue, float alpha = 1.0f);
    void set(const hsColorRGBA& init);

    void read(hsStream* S);
    void write(hsStream* S);
    void readRGB(hsStream* S);
    void writeRGB(hsStream* S);
    void prcWrite(pfPrcHelper* prc);
    void prcParse(const pfPrcTag* tag);
};

DllStruct hsColor32 {
public:
    union {
        struct {
            unsigned char b, g, r, a;
        };
        unsigned int color;
    };

    hsColor32(unsigned int color32 = 0xFF000000);
    hsColor32(unsigned char red, unsigned char green, unsigned char blue,
              unsigned char alpha = 255);
    hsColor32(const hsColor32& init);

    void read32(hsStream* S);
    void write32(hsStream* S);
    void readRGB8(hsStream* S);
    void writeRGB8(hsStream* S);
    void readRGBA8(hsStream* S);
    void writeRGBA8(hsStream*S );
    void prcWrite(pfPrcHelper* prc);
    void prcParse(const pfPrcTag* tag);
};

#endif