import RPi.GPIO as GPIO
import time
from threading import Thread
import os
import sys

def pwm():
    GPIO.setmode(GPIO.BOARD)

    GPIO.setup(16, GPIO.OUT)
    GPIO.setup(18, GPIO.OUT)
    GPIO.setup(22, GPIO.IN)

    i = 0
    dutycycle = 0
    p = GPIO.PWM(16,50)
    p.start(0)
    while True:
        try:
            dutycycle_input = float(input('Drehzahl? \n'))
            while i < 50:
                dutycycle += (dutycycle_input - dutycycle)*0.1
                i += 1
                time.sleep(.1)
                p.ChangeDutyCycle(dutycycle)
                print(dutycycle)
            dutycycle = dutycycle_input
            
            i = 0
            if dutycycle < 0:
                GPIO.output(18, GPIO.HIGH)
                dutycycle = abs(dutycycle)
                
            else: 
                GPIO.output(18, GPIO.LOW)
            if GPIO.input(22) == True:
                print('stk fault')                
                #exit()
            else:
                p.ChangeDutyCycle(dutycycle)
            

def ADS_M:
    DRDY = 11
    SCLK = 13
    PWDN = 15

    #std GPIO for DataReady is 29, for SerialClock 31
    GPIO.setmode(GPIO.BOARD)
    #GPIO.setwarnings(False)

    #declares which pin is In which is Out
    GPIO.setup(DRDY, GPIO.IN)
    GPIO.setup(SCLK, GPIO.OUT)
    GPIO.setup(PWDN, GPIO.OUT)

    ausgabe = ''
    i = 0
    actual = 0
    flag = 0

    def zwokompdez(a):
    #wandelt ein zweierkomplement in dezimal um
        a = '0b' + a
        if a[2] == '0':
            return(int(a,2))
        else:
            b = len(a)
            a = int(a,2)        
            a = a-1
            a = bin(a)
            if len(a) < b:
                a = -(pow(2, len(a)-1)/2)
                return(int(a))
            else:    
                b = '0b'
                i = 2
                while i < len(a):
                    if a[i] == '0':
                        b += '1'
                    else:
                        b += '0'
                    i += 1           
                a = b
                a = int(a,2)
                a =- a    
                return(a)

    def ISR_DRDY(channel):
        #retrieves data from ADS1232
        global ausgabe
        global i
        global actual
        global flag
        actual = time.time()
        ausgabe = ''
        #Data is retrieved once
        #print('in der ISR')
        GPIO.remove_event_detect(DRDY)
        
        while i < 24:
            #print('gathering data')
            ausgabe = str(ausgabe)
            GPIO.output(SCLK, 1)
            time.sleep(.0001)
            ausgabe += str(GPIO.input(DRDY))
            GPIO.output(SCLK, 0)
            time.sleep(.0001)
            i += 1
            if i == 24:
                flag = 1
        #Forces DRDY to "high", so a data ready state can be recognized
        GPIO.output(SCLK, 1)
        time.sleep(.0001)
        GPIO.output(SCLK, 0)
        
        
        #print( actual - time.time() )
        #print(ausgabe)
        i = 0
        #ausgabe = ''
        actual = 0
        GPIO.add_event_detect(DRDY, GPIO.FALLING, callback = ISR_DRDY)

    def CALIB(channel):
        #calibrates the ADS1232 and initiates data retrieval
        global ausgabe
        global i
        global actual
        actual = time.time()
        #Data is retrieved once
        print('in der CALIB')
        #event detect must be removed to stop 'ghost' clock signals...don't ask me where they came from but they do come if you outcomment the next line....
        GPIO.remove_event_detect(DRDY)
        
        while i < 24:
            ausgabe = str(ausgabe)
            GPIO.output(SCLK, 1)
            #time.sleep(.00001)
            ausgabe += str(GPIO.input(DRDY))
            GPIO.output(SCLK, 0)
            #time.sleep(.00001)
            i += 1
        #two additional Sclk signals to initiate calibration
        GPIO.output(SCLK, 1)
        time.sleep(.00001)
        GPIO.output(SCLK, 0)
        time.sleep(.00001)
        GPIO.output(SCLK, 1)
        time.sleep(.00001)
        GPIO.output(SCLK, 0)

        #print(ausgabe)
        i = 0
        ausgabe = ''
        actual = 0
        # initiate the 'real' event detect routine to retriev data
        GPIO.add_event_detect(DRDY, GPIO.FALLING, callback = ISR_DRDY)

    timestr = time.strftime("%Y-%m-%d-%H-%M")

    fobj_out = open('Messwerte' + timestr + '.txt',"w")
    fobj_out.write('Time		Moment \n')
    fobj_out.write('---------------------------------------------------')   

    #init der Messhardware
    GPIO.output(PWDN, 0)
    time.sleep(1)
    GPIO.output(PWDN, 1)
    GPIO.output(SCLK, 0)

    #Kalibrierfunktion
    GPIO.add_event_detect(DRDY, GPIO.FALLING, callback = CALIB)

    try:
        while True:
            if flag == 1:
                print(ausgabe)
                #ausgabe = (zwokompdez(ausgabe))
                #timestr = time.strftime("%H-%M-%S")
                #print(ausgabe)
                fobj_out.write(ausgabe + '\n')
                flag = 0
                #ausgabe = ''
            #else: time.sleep(.1)
   








t_pwm = Thread(target=pwm; args=())
t_ADS_M = Thread(target=ADS_M; args=())
t_ADS_F = Thread(target=pwm; args=())

t_pwm.start()
t_ADS_M.start()
t_ADS_F.start()

    



