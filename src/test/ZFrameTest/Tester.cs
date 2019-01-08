using System;
using System.Threading;
using System.Threading.Tasks;
using Agebull.Common.Configuration;
using Agebull.Common.Ioc;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.ZeroApi;
using ZeroMQ;

namespace RpcTest
{
    internal class Tester
    {
        static string address = "tcp://192.168.240.132:8000";
        public static void StartTest()
        {
            address = "tcp://192.168.240.132:8000";
            //TestFrame("��֡", ZeroOperatorStateType.FrameInvalid);
            //TestFrame("��֡", ZeroOperatorStateType.FrameInvalid, new byte[0]);
            //TestFrame("��֡", ZeroOperatorStateType.FrameInvalid, "".ToZeroBytes());
            //TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[1] { 1 });
            //TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[1] { 1 });
            //TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[1] { 1 }, new byte[1] { 1 });
            //TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[] { 1, 1 }, new byte[1] { 0 });
            //TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[] { 1, 1 }, new byte[1] { 1 });
            ////TestFrame("����֡", ZeroOperatorStateType.FrameInvalid, new byte[] { 1, 1 }, new byte[0]);
            //TestFrame("��������֡", ZeroOperatorStateType.ArgumentInvalid, new byte[] { 1, 1, (byte)'c', 0 }, new byte[1] { 1 });
            ////״̬����
            //for (byte state = 1; state < byte.MaxValue; state++)
            //{
            //    TestFrame($"״̬֡({(ZeroByteCommand)state})", ZeroOperatorStateType.ArgumentInvalid, new byte[] { 0, state, 0 });
            //}
            //TestFrame("��֡", ZeroOperatorStateType.ArgumentInvalid, new byte[] { 1, 1, (byte)'c', 0 }, new byte[0]);

            address = "tcp://192.168.240.132:8010";
            //״̬����
            for (byte state = 1; state < byte.MaxValue; state++)
            {
                TestFrame($"״̬֡({(ZeroOperatorStateType)state})", ZeroOperatorStateType.ArgumentInvalid,"agebull".ToZeroBytes(), new byte[] { 0, state, 0 });
            }
            Console.ReadLine();
        }
        static void TestFrame(string title, ZeroOperatorStateType state, params byte[][] frames)
        {
            Console.Error.Write(title);
            Console.ForegroundColor = ConsoleColor.Red;
            TestFrameInner(title, state, frames);
            Console.Error.WriteLine();
            Console.Error.WriteLine("**----**");
            Console.ResetColor();
        }
        static void TestFrameInner(string title, ZeroOperatorStateType state, byte[][] frames)
        {
            using (var socket = ZSocket.CreateClientSocket(address, ZSocketType.DEALER,ZeroIdentityHelper.CreateIdentity()))
            {
                socket.SetOption(ZSocketOption.RCVTIMEO, 30000);
                if (!socket.SendTo(frames))
                {
                    Console.Error.Write(" : Send Error");
                    return;
                }
                var result = socket.Receive();
                if (!result.InteractiveSuccess)
                    Console.Error.WriteLine(" : Receive Error");
                if (result.State == state)
                {
                    Console.ForegroundColor = ConsoleColor.Blue;
                    Console.Write($"(success) : {state}");
                }
                else
                    Console.Error.Write($"(bad) : {result.State}");
            }
        }
    }
}