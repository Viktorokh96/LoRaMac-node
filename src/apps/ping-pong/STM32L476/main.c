/*!
 * \file      main.c
 *
 * \brief     Ping-Pong implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "uart.h"
#include "delay.h"
#include "timer.h"
#include "radio.h"
#include "logging.h"

#include "rtc-board.h"

#define REGION_EU433

//#define CHECK_RTC 1

#if defined( REGION_AS923 )

#define RF_FREQUENCY                                923000000 // Hz

#elif defined( REGION_AU915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_CN470 )

#define RF_FREQUENCY                                470000000 // Hz

#elif defined( REGION_CN779 )

#define RF_FREQUENCY                                779000000 // Hz

#elif defined( REGION_EU433 )

#define RF_FREQUENCY                                433000000 // Hz

#elif defined( REGION_EU868 )

#define RF_FREQUENCY                                868000000 // Hz

#elif defined( REGION_KR920 )

#define RF_FREQUENCY                                920000000 // Hz

#elif defined( REGION_IN865 )

#define RF_FREQUENCY                                865000000 // Hz

#elif defined( REGION_US915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_US915_HYBRID )

#define RF_FREQUENCY                                915000000 // Hz

#else
    #error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             14        // dBm

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#elif defined( USE_MODEM_FSK )

#define FSK_FDEV                                    25000     // Hz
#define FSK_DATARATE                                50000     // bps
#define FSK_BANDWIDTH                               50000     // Hz
#define FSK_AFC_BANDWIDTH                           83333     // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#else
    #error "Please define a modem in the compiler options."
#endif

typedef enum
{
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
}States_t;

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led4;
extern Gpio_t Led5;

/*!
 * Uart2 Handle
 */
extern Uart_t Uart2;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

/* Прослушка на предмет активности канала */
void OnCadDone( bool channelActivityDetected );

#if defined( CHECK_RTC )
void Timer1TestCallback( void )
{
	Log("In 1 callback!\n"); DelayMs(20);
}

void Timer2TestCallback( void )
{
	Log("In 2 callback!\n"); DelayMs(20);
}

void Timer3TestCallback( void )
{
	Log("In 3 callback!\n"); DelayMs(20);
}

void CheckRTC( void )
{
	TimerTime_t time;
	TimerEvent_t tevent1, tevent2, tevent3;

	Log("\n---RTC CHECK---\n");

	Log("Delay 10 ms...\n"); 
	Log("RtcGetTimerValue: %i\n", RtcGetTimerValue());
	DelayMs( 10 );
	Log("RtcGetTimerValue: %i\n", RtcGetTimerValue());
	DelayMs( 10 );
	Log("RtcGetTimerValue: %i\n", RtcGetTimerValue());

	Log("\n---TIMERS CHECK---\n");


	Log("Delay 10 ms...\n"); 
	Log("Delay 100 ms...\n"); 
	Log("Delay 1000 ms...\n"); 
	Log("TimerGetCurrentTime: %i\n", time=TimerGetCurrentTime());
	DelayMs( 10 );
	Log("TimerGetElapsedTime: %i\n", TimerGetElapsedTime( time ));
	DelayMs( 100 );
	Log("TimerGetElapsedTime: %i\n", TimerGetElapsedTime( time ));
	DelayMs( 1000 );
	Log("TimerGetElapsedTime: %i\n", TimerGetElapsedTime( time ));

	Log("------------\n");

	TimerInit( &tevent1, Timer1TestCallback );
	TimerInit( &tevent2, Timer2TestCallback );
	TimerInit( &tevent3, Timer3TestCallback );
	TimerSetValue( &tevent1, 1000 );
	TimerSetValue( &tevent2, 5000 );
	TimerSetValue( &tevent3, 10000 );

	Log("Timer options: {Timestamp:%i, ReloadValue:%i, IsRunning:%i}\n", \
		tevent1.Timestamp, tevent1.ReloadValue, tevent1.IsRunning);

	Log("TimerStart...\n"); 
	TimerStart( &tevent1 );
	TimerStart( &tevent2 );
	TimerStart( &tevent3 );

	Log("Delay 15000 ms...\n"); DelayMs( 15000 );
	Log("Timer options: {Timestamp:%i, ReloadValue:%i, IsRunning:%i}\n", \
		tevent1.Timestamp, tevent1.ReloadValue, tevent1.IsRunning);

	Log("TimerStop...\n"); 
	TimerStop( &tevent1 );
	TimerStop( &tevent2 );
	TimerStop( &tevent3 );

	Log("Timer options: {Timestamp:%i, ReloadValue:%i, IsRunning:%i}\n", \
		tevent1.Timestamp, tevent1.ReloadValue, tevent1.IsRunning);
}
#endif

void TestRadio()
{
	uint8_t addr, data;

	Log("Reading first default 16 register values...\n"); DelayMs(20);

	for (addr = 0; (addr & 0xf0) == 0; ++addr) {
		data = Radio.Read(addr);
		Log("Address [%x] - Data [%x]\n", addr, data); DelayMs(20);
	}

	Log("------\n"); DelayMs(20);

	Log("Writing values 123456789ABCDEF to first 16 registers...\n"); DelayMs(20);

	for (addr = 0; (addr & 0xf0) == 0; ++addr)
		Radio.Write(addr, addr);

	Log("Reading first default 16 register values...\n"); DelayMs(20);

	for (addr = 0; (addr & 0xf0) == 0; ++addr) {
		data = Radio.Read(addr);
		Log("Address [%x] - Data [%x]\n", addr, data); DelayMs(20);
	}
}

/**
 * Main application entry point.
 */
int main( void )
{
    bool isMaster = true;
    uint8_t i;

    // Target board initialization
    BoardInitMcu( );
    BoardInitPeriph( );

    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
	RadioEvents.CadDone = OnCadDone;

	Log("Radio init...\n");
    Radio.Init( &RadioEvents );
	DelayMs(100);

    Radio.SetChannel( RF_FREQUENCY );

#if defined( USE_MODEM_LORA )

    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

#elif defined( USE_MODEM_FSK )

    Radio.SetTxConfig( MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                                  FSK_DATARATE, 0,
                                  FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, 0, 3000 );

    Radio.SetRxConfig( MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                                  0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                                  0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                                  0, 0,false, true );

#else
    #error "Please define a frequency band in the compiler options."
#endif

#if defined( CHECK_RTC )
	while( 1 )
	{
		CheckRTC();
		DelayMs(30000);
	}
#endif

    Radio.Rx( RX_TIMEOUT_VALUE );

	//Radio.StartCad(); while(1);
	
	//Buffer[0] = 'P';
	//Buffer[1] = 'I';
	//Buffer[2] = 'N';
	//Buffer[3] = 'G';
	// We fill the buffer with numbers for the payload*/
	//for( i = 4; i < BufferSize; i++ )
	//{
		//Buffer[i] = i - 4;
	//}
	//Radio.Send( Buffer, BufferSize );
	//while(1);

    while( 1 )
    {
        switch( State )
        {
        case RX:
            if( isMaster == true )
            {
                if( BufferSize > 0 )
                {
                    if( strncmp( ( const char* )Buffer, ( const char* )PongMsg, 4 ) == 0 )
                    {
                        // Indicates on a LED that the received frame is a PONG
                        GpioWrite( &Led5, GpioRead( &Led5 ) ^ 1 );

                        // Send the next PING frame
                        Buffer[0] = 'P';
                        Buffer[1] = 'I';
                        Buffer[2] = 'N';
                        Buffer[3] = 'G';
                        // We fill the buffer with numbers for the payload
                        for( i = 4; i < BufferSize; i++ )
                        {
                            Buffer[i] = i - 4;
                        }
                        DelayMs( 1 );
                        Radio.Send( Buffer, BufferSize );

						Log("PING sended...\n"); DelayMs( 100 );
                    }
                    else if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
                    { // A master already exists then become a slave
                        isMaster = false;
                        GpioWrite( &Led4, 1 ); // Set LED off
						Log("Waiting RX...\n"); DelayMs(20);
                        Radio.Rx( RX_TIMEOUT_VALUE );
                    }
                    else // valid reception but neither a PING or a PONG message
                    {    // Set device as master ans start again
                        isMaster = true;
						Log("Waiting RX...\n"); DelayMs(20);
                        Radio.Rx( RX_TIMEOUT_VALUE );
                    }
                }
            }
            else
            {
                if( BufferSize > 0 )
                {
                    if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
                    {
                        // Indicates on a LED that the received frame is a PING
                        GpioWrite( &Led5, GpioRead( &Led5 ) ^ 1 );

                        // Send the reply to the PONG string
                        Buffer[0] = 'P';
                        Buffer[1] = 'O';
                        Buffer[2] = 'N';
                        Buffer[3] = 'G';
                        // We fill the buffer with numbers for the payload
                        for( i = 4; i < BufferSize; i++ )
                        {
                            Buffer[i] = i - 4;
                        }
                        DelayMs( 1 );
                        Radio.Send( Buffer, BufferSize );

						Log("PONG sended...\n"); DelayMs( 100 );
                    }
                    else // valid reception but not a PING as expected
                    {    // Set device as master and start again
                        isMaster = true;
						Log("Waiting RX...\n"); DelayMs(10);
                        Radio.Rx( RX_TIMEOUT_VALUE );
                    }
                }
            }
            State = LOWPOWER;
            break;
        case TX:
            // Indicates on a LED that we have sent a PING [Master]
            // Indicates on a LED that we have sent a PONG [Slave]
            //GpioWrite( &Led4, GpioRead( &Led4 ) ^ 1 );
			//Log("Waiting RX...\n"); DelayMs(20);
            //Radio.Rx( RX_TIMEOUT_VALUE );
            State = LOWPOWER;
            break;
        case RX_TIMEOUT:
        case RX_ERROR:

			Log("RX_TIMEOUT or RX_ERROR...\n"); DelayMs( 100 );

            if( isMaster == true )
            {
                // Send the next PING frame
                Buffer[0] = 'P';
                Buffer[1] = 'I';
                Buffer[2] = 'N';
                Buffer[3] = 'G';
                for( i = 4; i < BufferSize; i++ )
                {
                    Buffer[i] = i - 4;
                }
                DelayMs( 1 );
                Radio.Send( Buffer, BufferSize );

				Log("PING sended...\n"); DelayMs( 100 );
            }
            else
            {
				Log("Waiting RX...\n"); DelayMs(20);
                Radio.Rx( RX_TIMEOUT_VALUE );
            }
            State = LOWPOWER;
            break;
        case TX_TIMEOUT:
			Log("Waiting RX...\n"); DelayMs(20);
            Radio.Rx( RX_TIMEOUT_VALUE );
            State = LOWPOWER;
            break;
        case LOWPOWER:
        default:
            // Set low power
            break;
        }

        //TimerLowPowerHandler( );
    }
}

void OnTxDone( void )
{
	//uint8_t i = 0;

    Radio.Sleep( );
    State = TX;

	Log("OnTxDone. New state = TX\n"); DelayMs( 100 );
	GpioWrite( &Led4, GpioRead( &Led4 ) ^ 1 );
	Log("Waiting RX...\n"); DelayMs(20);
	Radio.Rx( RX_TIMEOUT_VALUE );

	//Buffer[0] = 'P';
	//Buffer[1] = 'I';
	//Buffer[2] = 'N';
	//Buffer[3] = 'G';
	// We fill the buffer with numbers for the payload
	//for( i = 4; i < BufferSize; i++ )
	//{
		//Buffer[i] = i - 4;
	//}
	//Radio.Send( Buffer, BufferSize );
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Sleep( );
    BufferSize = size;
    memcpy( Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue = snr;
    State = RX;

	Log("OnRxDone: Payload size=%i RSSI=%i New state = RX\n", size, rssi); 
	DelayMs( 100 );
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    State = TX_TIMEOUT;

	Log("OnTxTimeout... New state = TX_TIMEOUT\n"); DelayMs( 100 );
}

void OnRxTimeout( void )
{
    Radio.Sleep( );
    State = RX_TIMEOUT;

	Log("OnRxTimeout... New state = RX_TIMEOUT\n"); DelayMs( 100 );
}

void OnRxError( void )
{
    Radio.Sleep( );
    State = RX_ERROR;

	Log("OnRxError! New state = RX_ERROR\n"); DelayMs( 100 );
}

void OnCadDone( bool channelActivityDetected )
{
	if (channelActivityDetected) 
	{
		Log("OnCadDone. Channel ACTIVITY DETECTED!!!\n"); DelayMs( 100 );
	}
	else
	{
		Log("OnCadDone. Channel activity not detected...\n"); DelayMs( 100 );
		Radio.StartCad();
	}
}
