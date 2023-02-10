   channel_struct *chan = &SPU->chan[channel];

   chan->sampinc = (16777216 / (0x10000 - (double)chan->timer)) / 44100;

   switch(chan->format)
   {
      case 0: // 8-bit
         chan->loopstart = chan->loopstart << 2;
         chan->length = (chan->length << 2) + chan->loopstart;
         chan->sampcnt = 0;
         break;
      case 1: // 16-bit
         chan->loopstart = chan->loopstart << 1;
         chan->length = (chan->length << 1) + chan->loopstart;
         chan->sampcnt = 0;
         break;
      case 2: // ADPCM
      {
         chan->pcm16b = (s16)((chan->buf8[1] << 8) | chan->buf8[0]);
         chan->index = chan->buf8[2] & 0x7F;
         chan->lastsampcnt = 7;
         chan->sampcnt = 8;
         chan->loopstart = chan->loopstart << 3;
         chan->length = (chan->length << 3) + chan->loopstart;
         break;
      }
      case 3: // PSG
      {
         break;
      }
      default: break;
   }

static INLINE void FetchADPCMData(channel_struct *chan, s32 *data)
{
   u8 data4bit;
   int diff;
   int i;

   if (chan->lastsampcnt == (int)chan->sampcnt)
   {
      // No sense decoding, just return the last sample
      *data = (s32)chan->pcm16b;
      return;
   }

   for (i = chan->lastsampcnt+1; i < (int)chan->sampcnt+1; i++)
   {
      if (i & 0x1)
         data4bit = (chan->buf8[i >> 1] >> 4) & 0xF;
      else
         data4bit = chan->buf8[i >> 1] & 0xF;

      diff = ((data4bit & 0x7) * 2 + 1) * adpcmtbl[chan->index] / 8;
      if (data4bit & 0x8)
         diff = -diff;

      chan->pcm16b = (s16)MinMax(chan->pcm16b+diff, -0x8000, 0x7FFF);
      chan->index = MinMax(chan->index+indextbl[data4bit & 0x7], 0, 88);
   }

   chan->lastsampcnt = chan->sampcnt;
   *data = (s32)chan->pcm16b;
}

static INLINE void MixLR(channel_struct *chan, s32 data)
{
   if (data)
   {
      data = ((data * chan->vol) / 127) >> chan->datashift;
      SPU->sndbuf[SPU->bufpos<<1] += data * (127 - chan->pan) / 127;
      SPU->sndbuf[(SPU->bufpos<<1)+1] += data * chan->pan / 127;
   }
}

static INLINE void TestForLoop2(channel_struct *chan)
{
   chan->sampcnt += chan->sampinc;

   if (chan->sampcnt > (double)chan->length)
   {
      // Do we loop? Or are we done?
      if (chan->repeat == 1)
      {
         chan->sampcnt = (double)chan->loopstart; // Is this correct?
         chan->pcm16b = (s16)((chan->buf8[1] << 8) | chan->buf8[0]);
         chan->index = chan->buf8[2] & 0x7F;
         chan->lastsampcnt = 7;
      }
      else
      {
         chan->status = CHANSTAT_STOPPED;
         MMU.ARM7_REG[0x403 + ((((u32)chan-(u32)SPU->chan) / sizeof(channel_struct)) * 0x10)] &= 0x7F;
         SPU->bufpos = SPU->buflength;
      }
   }
}

void SPU_ChanUpdateADPCMLR(channel_struct *chan)
{
   for (; SPU->bufpos < SPU->buflength; SPU->bufpos++)
   {
       s32 data;

       // fetch data from source address
       FetchADPCMData(chan, &data);

       MixLR(chan, data);

       // check to see if we're passed the length and need to loop, etc.
       TestForLoop2(chan);
   }
}
