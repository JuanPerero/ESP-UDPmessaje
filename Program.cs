using System;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace shutdownProyect
{
    class Program
    {
        static int listenPort = 8889;
        //static string linuxcommand = "System.Diagnostics.Process.Start(\"Shutdown\", \\");";
        //static string windowcommand = "        process.StartInfo.FileName = "shutdown";
        //process.StartInfo.Arguments = "-h now";";

        static void Main(string[] args)
        {
            programa actuador = new programa();

            UdpClient listener = new UdpClient(listenPort);
            IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, listenPort);
            try
            {
                while (true)
                {
                    byte[] bytes = listener.Receive(ref groupEP);
                    if (bytes[0] == 'S' && bytes[1] == 'H' && bytes[2] == 'U' && bytes[3] == 'T')
                    {
                        Console.WriteLine("APAGAR");
                        int stop = 1;                       
                        //actuador.rutina();                
                    }
                    else
                    {
                        Console.WriteLine("algo");
                    }
                    //Console.WriteLine($"Received broadcast from {groupEP} :");
                    //Console.WriteLine($" {Encoding.ASCII.GetString(bytes, 0, bytes.Length)}");
                }
            }
            catch (SocketException e)
            {
                Console.WriteLine(e);
            }
            finally
            {
                listener.Close();
            }
        }
    }





    class programa
    {
        public delegate void deleg();
        public deleg rutina;

        public programa()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                rutina = linshutdown;
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                rutina = winshutdown;
            }
        }
        void winshutdown()
        {
            try
            {
                System.Diagnostics.Process process = new System.Diagnostics.Process();
                process.StartInfo.FileName = "shutdown";
                process.StartInfo.Arguments = "-h now";
            }
            catch (SocketException e)
            {
                Console.WriteLine(e);
                Console.WriteLine("Controlar ejecucion como Administrador");
            }
        }

        void linshutdown()
        {
            try
            {
                System.Diagnostics.Process process = new System.Diagnostics.Process();
                process.StartInfo.FileName = "/usr/bin/sudo";
                process.StartInfo.Arguments = "/sbin/shutdown -h now";
            }
            catch (SocketException e)
            {
                Console.WriteLine(e);
                Console.WriteLine("Controlar ejecucion como SUDO");
            }
        }
    }


}
