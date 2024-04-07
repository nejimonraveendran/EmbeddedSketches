using System;
using System.Device.Gpio;
using System.Diagnostics;
using System.Threading;

namespace HelloWorld
{
    public class Program
    {
        private static GpioController _gpioController;

        public static void Main()
        {
            _gpioController = new GpioController();

            GpioPin led = _gpioController.OpenPin(33, PinMode.Output); // 33 is the on-board LED

            led.Write(PinValue.Low);

            while (true)
            {
                led.Toggle();
                Thread.Sleep(1000);
            }


        }
    }
}
