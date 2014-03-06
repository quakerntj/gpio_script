#ifndef GPIO_H
#define GPIO_H

class Gpio {
public:
    Gpio(int pin);
    ~Gpio();
    inline int init() {return exportGpio();};

private:
    int mPin;
    int mIsExported;
    int mDirection;
    int mEdge;
    int mIsActiveLow;
    int mError;

public:
    inline int getError() {return mError;}
    int exportGpio();
    int unexportGpio();
    int getDirection();
    int setDirection(int dir);
    int getEdgeTrigger();
    int setEdgeTrigger(int trigger);
    int getActiveLow();
    int setActiveLow(int isLow);

public:
    int read();
    int write(int boolean);

private:
    // Disallow copy and sign
    Gpio(const Gpio&);
    Gpio();
};

#endif
