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
    enum {
        UNKNOWN = -1
    };
    enum {
        IN = 0,
        OUT = 1,
    };

    enum {
        LOW = 0,
        HIGH = 1,
    };

    enum {
        NONE = 0,
        RISING = 5,
        FALLING = 12,
        BOTH = 17
    };

public:
    inline int getPin() {return mPin;}
    inline bool getIsExported() {return mIsExported == 1;}
    inline int getDir() {return mDirection;}
    inline int getEdge() {return mEdge;}
    inline int getIsActiveLow() {return mIsActiveLow;}
    inline int getError() {return mError;}
    inline const char* getEdgeName(int edge) {
        const char name[] = "none\0rising\0falling\0both";
        return name + edge;
    }

public:
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

public:
    static Gpio* getInstance(int pin);

private:
    // Disallow copy and sign
    Gpio(const Gpio&);
    Gpio();
};

extern int gGpios;
extern int gBase;
int readStringFromN(char * in, size_t size, const char * path);
int initChip();
void finishGpios();

#endif
