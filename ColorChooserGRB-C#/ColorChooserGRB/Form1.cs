using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ColorChooserGRB
{
    public partial class Form1 : Form
    {

        [DllImport("user32.dll")]
        static extern bool GetCursorPos(ref Point lpPoint);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, SetLastError = true, ExactSpelling = true)]
        public static extern int BitBlt(IntPtr hDC, int x, int y, int nWidth, int nHeight, IntPtr hSrcDC, int xSrc, int ySrc, int dwRop);

        static bool blinkSleep;
        static bool mus;
        static bool blinkingDownOrUp;
        static bool clicked = false;
        static int intensity = 141; //default start value for intensity
        public static SerialPort LPC1114 = new SerialPort("COM4", 38400); //connection to LPC1114
        static String textLPC = "";
        static Color prevColor;
        Bitmap screenPixel = new Bitmap(1, 1, PixelFormat.Format32bppArgb); //current color located under cursor
        static ColoRand ts = new ColoRand(); //generate random color
        List<Color> currentColor = new List<Color>();
        static bool cycleColor = false;
        static Label connectionMessage;
        static Color cycleColorOne;
        static Color cycleColorTwo;
        int startpos = 0;
        int NUMBER_OF_LEDS = 59;
        String theColor;
        byte[] colorBytes = new byte[3];
        BackgroundWorker m_oWorker;

        public Form1()
        {
            InitializeComponent();


            connectionMessage = label2;
            LPC1114.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

            try
            {
                LPC1114.Open();
            }
            catch (System.IO.IOException e)
            {
                MessageBox.Show("Connect LPC1114 programmer on COM4");
            }
            
            trackBar1.MouseDown += (s,
                                    e) =>
            {
                clicked = true;
            };
            trackBar1.MouseUp += (s,
                                    e) =>
            {
                if (!clicked)
                    return;

                clicked = false;
                change_brightness(trackBar1.Value);
            };

            //background worker to do things apart from the GUI
            m_oWorker = new BackgroundWorker();
            m_oWorker.DoWork += new DoWorkEventHandler(m_oWorker_DoWork);
            m_oWorker.WorkerReportsProgress = false; //useless
            m_oWorker.WorkerSupportsCancellation = true;
        }

        public static Color ChangeColorBrightness(Color color, float correctionFactor)
        {
            float red = (float)color.R;
            float green = (float)color.G;
            float blue = (float)color.B;

            if (correctionFactor < 0)
            {
                correctionFactor = 1 + correctionFactor;
                red *= correctionFactor;
                green *= correctionFactor;
                blue *= correctionFactor;
            }
            else
            {
                red = (255 - red) * correctionFactor + red;
                green = (255 - green) * correctionFactor + green;
                blue = (255 - blue) * correctionFactor + blue;
            }

            return Color.FromArgb(color.A, (int)red, (int)green, (int)blue);

        }

        private void change_brightness(int value)
        {
            if (!clicked)
            {
                float normalized_value = ((value - 0f) / (255f - 0f)) * (1f - -1f) + -1f; //generate value between -1 and 1, from range 0 to 255

                List<Color> modiCols = new List<Color>();
                if (currentColor != null)
                {
                    foreach (Color col in currentColor)
                    {
                        modiCols.Add(ChangeColorBrightness(col, normalized_value));

                    }
                    // coltmp = modiCols;
                    write_color_stream(modiCols);
                }
            }
        }

        public Color GetColorAt(Point location)
        {
            using (Graphics gdest = Graphics.FromImage(screenPixel))
            {
                using (Graphics gsrc = Graphics.FromHwnd(IntPtr.Zero))
                {
                    IntPtr hSrcDC = gsrc.GetHdc();
                    IntPtr hDC = gdest.GetHdc();
                    int retval = BitBlt(hDC, 0, 0, 1, 1, hSrcDC, location.X, location.Y, (int)CopyPixelOperation.SourceCopy);
                    gdest.ReleaseHdc();
                    gsrc.ReleaseHdc();
                }
            }

            return screenPixel.GetPixel(0, 0);
        }


        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;

            string indata = sp.ReadExisting();
            textLPC = indata;

            if (textLPC.Contains("LPC1114"))
            {
                connectionMessage.ForeColor = Color.Blue;
                Action updateLabel = () => connectionMessage.Text = "CONNECTED";
                connectionMessage.Invoke(updateLabel);

            }
        }

        private void write_color_stream(List<Color> theCols)
        {
            // coltmp = theCols; //keep track of current colors in led strip
            if (theCols != null)
            {
                foreach (Color col in theCols)
                {
                    byte[] tempCol = new byte[3];
                    tempCol[0] = col.G;
                    tempCol[1] = col.R;
                    tempCol[2] = col.B;
                    LPC1114.Write(tempCol, 0, 3);
                    //  String text = LPC1114.ReadLine();
                    //   if (text != "") {
                    //     MessageBox.Show("The text was: " + text);
                    //   }

                }
                // while (true) {
                //     if (textLPC.Contains("SUCCESS"))
                //      {
                //      MessageBox.Show("Color applied");
                //      break;
                //    }
                //  }
            }
            //  Application.Exit();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            write_color_stream(currentColor);
        }

        public static List<Color> GetGradientColors(Color start, Color end, int steps)
        {
            return GetGradientColors(start, end, steps, 0, steps - 1);
        }

        public static List<Color> GetGradientColors(Color start, Color end, int steps, int firstStep, int lastStep)
        {
            var colorList = new List<Color>();
            if (steps <= 0 || firstStep < 0 || lastStep > steps - 1)
                return colorList;

            double aStep = (end.A - start.A) / steps;
            double rStep = (end.R - start.R) / steps;
            double gStep = (end.G - start.G) / steps;
            double bStep = (end.B - start.B) / steps;

            for (int i = firstStep; i < lastStep; i++)
            {
                var a = start.A + (int)(aStep * i);
                var r = start.R + (int)(rStep * i);
                var g = start.G + (int)(gStep * i);
                var b = start.B + (int)(bStep * i);
                colorList.Add(Color.FromArgb(a, r, g, b));
            }

            return colorList;
        }

        //set single color
        private void button2_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                List<Color> colooren = GetGradientColors(colorDialog1.Color, colorDialog1.Color, NUMBER_OF_LEDS + 1);
                currentColor = colooren;

                button1.BackColor = colorDialog1.Color;
            }
        }

        //set gradient color
        private void button3_Click(object sender, EventArgs e)
        {

            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                Color one, two;
                one = colorDialog1.Color;
                if (colorDialog1.ShowDialog() == DialogResult.OK)
                {
                    two = colorDialog1.Color;
                    List<Color> colooren = GetGradientColors(one, two, NUMBER_OF_LEDS + 1);
                    currentColor = colooren;
                    write_color_stream(colooren);
                }
            }
        }


        //set random gradient color
        private void button4_Click(object sender, EventArgs e)
        {
            Color one = ts.RandomColor();
            Color two = ts.RandomColor();
            List<Color> colooren = GetGradientColors(one, two, NUMBER_OF_LEDS + 1);
            currentColor = colooren;
            write_color_stream(colooren);

        }

        private void button5_Click(object sender, EventArgs e)
        {
            button5.Enabled = false;
            button6.Enabled = true;
            m_oWorker.RunWorkerAsync(); //start background worker, let the show begin..
        }

        void m_oWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            while (!m_oWorker.CancellationPending)
            {


                // Update the colors.
                List<Color> colors = new List<Color>();

                if (blinkSleep) //blinking up and down
                {
                    if (0 <= intensity && intensity <= 10)
                    {
                        blinkingDownOrUp = true; //blinking up

                    }
                    if (135 <= intensity && intensity <= 145)
                    {
                        blinkingDownOrUp = false; //blinking down
                    }

                    if (blinkingDownOrUp) //blinking up
                    {
                        intensity += 5;
                        change_brightness(intensity);
                    }
                    else
                    {
                        intensity -= 5;
                        change_brightness(intensity);
                    }
                }
                else if (cycleColor) //cycle through specifed colors
                {

                    List<Color> colooren = new List<Color>();

                    int otherLeds = 10;
                    if (startpos + otherLeds == NUMBER_OF_LEDS)
                    {
                        blinkingDownOrUp = false;
                    }
                    if (startpos == 0)
                    {
                        blinkingDownOrUp = true;
                    }

                    if (blinkingDownOrUp)
                    {
                        startpos++;
                    }
                    else
                    {
                        startpos--;
                    }
                    for (int i = 0; i < NUMBER_OF_LEDS; i++)
                    {
                        if (startpos <= i && i < startpos + otherLeds)
                        {
                            colooren.Add(cycleColorOne);
                        }
                        else
                        {
                            colooren.Add(cycleColorTwo);
                        }

                    }
                    write_color_stream(colooren);
                }

                else //start random color show
                {
                    Random _r = new Random();
                    for (int i = 0; i < NUMBER_OF_LEDS; i++)
                    {
                        colors.Add(Color.FromArgb(_r.Next(0, 255), _r.Next(0, 255), _r.Next(0, 255)));
                    }


                    //     Color one = ts.RandomColor();
                    //     Color two = ts.RandomColor();
                    //     List<Color> colooren = GetGradientColors(one, two, NUMBER_OF_LEDS + 1);
                    write_color_stream(colors);
                }

                Thread.Sleep(Convert.ToInt32(textBox1.Text));
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (m_oWorker.IsBusy)
            {
                m_oWorker.CancelAsync();
                button5.Enabled = true;
                button6.Enabled = false;
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            List<Color> colooren = GetGradientColors(Color.FromArgb(0, 0, 0), Color.FromArgb(0, 0, 0), NUMBER_OF_LEDS + 1);
            write_color_stream(colooren);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Point cursor = new Point();
            GetCursorPos(ref cursor);

            Color c = GetColorAt(cursor);

            this.BackColor = c;
            if (c != prevColor)
            {
                prevColor = c;
                List<Color> colooren = GetGradientColors(c, c, NUMBER_OF_LEDS + 1);
                write_color_stream(colooren);
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            if (!mus)
            {
                timer1.Start();
                mus = true;
            }
            else
            {
                timer1.Stop();
            }

        }

        private void button9_Click(object sender, EventArgs e)
        {
            if (blinkSleep == false)
            {
                blinkSleep = true;
                m_oWorker.RunWorkerAsync();
            }
            else
            {
                if (m_oWorker.IsBusy)
                {
                    m_oWorker.CancelAsync();
                    blinkSleep = false;
                }
            }

        }

        private void button11_Click(object sender, EventArgs e)
        {
            connectionMessage.ForeColor = Color.Red;
            Action updateLabel = () => connectionMessage.Text = "NOT CONNECTED";
            connectionMessage.Invoke(updateLabel);
            LPC1114.Close();
        }

        private void button10_Click(object sender, EventArgs e)
        {
            connectionMessage.ForeColor = Color.Green;
            Action updateLabel = () => connectionMessage.Text = "PRESS RESET BUTTON";
            connectionMessage.Invoke(updateLabel);
            LPC1114.Close();
            LPC1114.Open();
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) &&
                (e.KeyChar != '.'))
            {
                e.Handled = true;
            }

            // only allow one decimal point
            if ((e.KeyChar == '.') && ((sender as TextBox).Text.IndexOf('.') > -1))
            {
                e.Handled = true;
            }
        }

        private void button12_Click(object sender, EventArgs e)
        {
            if (cycleColor == false)
            {
                if (colorDialog1.ShowDialog() == DialogResult.OK)
                {
                    cycleColorOne = colorDialog1.Color;
                    if (colorDialog1.ShowDialog() == DialogResult.OK)
                    {
                        cycleColorTwo = colorDialog1.Color;
                        cycleColor = true;
                        m_oWorker.RunWorkerAsync();

                    }
                }
            }
            else
            {
                if (m_oWorker.IsBusy)
                {
                    m_oWorker.CancelAsync();
                    cycleColor = false;
                }
            }
        }

        //Open form2, and enable AmbiLight possibility
        private void button13_Click(object sender, EventArgs e)
        {
            Form2 ambilight = new Form2();
            ambilight.Show(); //show form
        }
    }
}

