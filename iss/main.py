#!/usr/bin/python3
from scipy.io import wavfile
import matplotlib.pyplot as plt
import numpy as np
import time
import scipy.signal

u3 = False
u4 = False
u5 = False
u5_dft = False
u6 = False
u7 = True
u7_idft = True
u8 = False
u13 = True
u15 = False

moff_sr, maskoff = wavfile.read("../audio/maskoff_tone.wav")
mon_sr, maskon = wavfile.read("../audio/maskon_tone.wav")
s_sr, sentence = wavfile.read("../audio/maskoff_sentence.wav")
son_sr, sentenceon = wavfile.read("../audio/maskon_sentence.wav")

def autocorrelate(sig):
    nulls = np.array([0 for x in range(sig.size)])
    b = np.concatenate((sig,nulls))
    results = []
    for i in range(sig.size):
        results.append(np.sum(sig*b[i:sig.size+i]))
    return np.array(results)

def myDFT(sig):
    result = []
    for k in range(sig.size):
        s = 0
        for n in range(sig.size):
            s += sig[n]*np.power(np.e,2*np.pi*(-1j)*k*n/sig.size)
        result.append(s)
    return np.array(result)

def myIDFT(sig):
    result = []
    for k in range(sig.size):
        s = 0
        for n in range(sig.size):
            s += sig[n]*np.power(np.e,2*np.pi*(1j)*n*k/sig.size)
        s /= sig.size
        result.append(s)
    return np.array(result)

def clip(frame):
    max70 = max([abs(x) for x in frame])*0.7
    return np.array([1 if x >= max70 else (-1 if x <= -max70 else 0) for x in frame])

def wav_from_dft(maskoffdft, maskondft, file):
    H = []
    mon = np.array(maskondft)
    moff = np.array(maskoffdft)
    for x in range(len(maskoffdft)): # Výpočet H
        H.append(mon[x]/moff[x])
    
    H = np.array(H)

    havg = np.average(np.abs(H),axis=0) # zprůměrování absolutních hodnot komplexních koeficientů DFT
    havg = havg[:512]

    padding = [0 for _ in range(512)]
    havg = np.concatenate((havg,padding))
    odezva = np.fft.ifft(havg,1024) # inverzní fourierova transformace

    # aplikace filtru
    sf = scipy.signal.lfilter(odezva.real,[1.0],sentence)
    sf = np.real(sf)
    sf = np.int16(sf)

    st = scipy.signal.lfilter(odezva.real,[1.0],maskoff)
    st = np.real(st)
    st = np.int16(st)

    # zápis výsledného wav souboru
    wavfile.write(f"../audio/sim_maskon_sentence{file}.wav",s_sr,sf)
    wavfile.write(f"../audio/sim_maskon_tone{file}.wav",moff_sr, st)

# Ruční výběr 1s vzorku signálu
hpon = np.array(maskon[16000:32000]) # 1.0 - 2.0s

#Výběr druhého vzorku pomocí korelace
cors = []
for i in range(0,len(maskoff)-16000):
    cors.append(np.correlate(hpon, maskoff[i:i+16000]))
startindex = cors.index(max(cors))
hpoff = np.array(maskoff[startindex:startindex+16000])

# Převod na floaty
hpoff = hpoff.astype(np.float)
hpon = hpon.astype(np.float)

# Ustřednění
hpoff -= np.mean(hpoff)
hpon -= np.mean(hpon)

# Normalizace
hpoff /= np.abs(hpoff).max()
hpon /= np.abs(hpon).max()

ramce_mon = list()
ramce_moff = list()

# rozčlenění na rámce
# 20ms rámce == 320 vzorků => 10ms posun = 160 vzorků
for i in range(99):
    ramce_mon.append(hpon[i*160:(i+2)*160])
    ramce_moff.append(hpoff[i*160:(i+2)*160])

if u3:

    # Rámce k vykreslení
    mon_krramec = ramce_mon[1]
    moff_krramec = ramce_moff[1]

    casy = [x/moff_sr for x in range(len(mon_krramec))]

    # Úkol 3
    plt.subplots_adjust(hspace=1)
    plt.subplot(212)
    plt.plot(casy, mon_krramec)
    plt.xlabel("time")
    plt.ylabel("y")
    plt.title("Rámec s rouškou")
    plt.subplot(211)
    plt.plot(casy, moff_krramec)
    plt.xlabel("time")
    plt.ylabel("y")
    plt.title("Rámec bez roušky")
    plt.show()

# Pole na hodnoty lagu
lagsoff = []
lagson = []

# Rámce k zobrazení
showid = 1
wramec_off = ramce_moff[showid]
wramec_on = ramce_mon[showid]

# Centrální klipování
clippedon = []
clippedoff = []

for r in ramce_moff:
    max70 = max([abs(x) for x in r])*0.7
    clippedoff.append(np.array([1 if x >= max70 else (-1 if x <= -max70 else 0) for x in r]))

for r in ramce_mon:
    max70 = max([abs(x) for x in r])*0.7
    clippedon.append(np.array([1 if x >= max70 else (-1 if x <= -max70 else 0) for x in r]))

clippedon = np.array(clippedon)
clippedoff = np.array(clippedoff)

prah = 32 # Práh pro lagy 500 MHz

# Autokorelace rámců
for r in clippedoff:
    lag = autocorrelate(r)[prah:].argmax()+prah
    freq = 1/lag * moff_sr
    lagsoff.append(freq)

for r in clippedon:
    lag = autocorrelate(r)[prah:].argmax()+prah
    freq = 1/lag * mon_sr
    lagson.append(freq)

print(f"Avg OFFMASK = {np.mean(lagsoff)}")
print(f"Avg ONMASK = {np.mean(lagson)}")
print(f"Variance OFFMASK = {np.var(lagsoff)}")
print(f"Variance ONMASK = {np.var(lagson)}")

if u4:
    # Úkol 4
    autoc = autocorrelate(clippedoff[showid]) # rámec k zobrazení
    lag = autoc[prah:].argmax()+prah
    casy = [x/mon_sr for x in range(len(clippedon[showid]))]
    plt.tight_layout()
    plt.subplots_adjust(hspace=1)
    plt.subplot(411)
    plt.title("Rámec 2")
    plt.plot(casy, wramec_off)
    #plt.plot(casy, wramec_on)
    plt.xlabel("Čas")
    plt.ylabel("y")
    plt.subplot(412)
    plt.title("Clipping rámce 2 se 70%")
    plt.plot(casy, clippedoff[showid])
    plt.xlabel("Vzorek")
    plt.ylabel("y")
    plt.subplot(413)
    plt.title("Autokorelace rámce 2")
    plt.plot(autoc)
    plt.axvline(prah, label="Práh", color="k", linewidth=0.5)
    plt.stem([lag],[autoc[lag]], label="Lag", markerfmt="ro", basefmt="b-")
    plt.legend()
    plt.xlabel("Vzorek")
    plt.ylabel("y")
    plt.subplot(414)
    plt.title("Základní frekvence rámců")
    plt.plot(lagsoff, label="bez roušky")
    plt.plot(lagson, label="s rouškou")
    plt.legend()
    plt.xlabel("Rámce")
    plt.ylabel("f0")
    plt.show()


maskondft = []
maskoffdft = []
maskoff_compare = []

maskondft_same = []
maskoffdft_same = []
x = [0 for x in range(1024-ramce_moff[0].size)]
c = 0
for r in ramce_moff:
    ram = np.concatenate((r,x))
    res = np.fft.fft(ram,1024)
    if np.isclose(lagsoff[c],lagson[c]):
        maskoffdft_same.append(res)
    maskoffdft.append(res)
    if u5_dft:
        myres = myDFT(ram)
        maskoff_compare.append(myres)
        print(f"Done {c}/99")
    c += 1

c = 0
for r in ramce_mon:
    ram = np.concatenate((r,x))
    res = np.fft.fft(ram,1024)
    if np.isclose(lagsoff[c],lagson[c]):
        maskondft_same.append(res)
    c += 1
    maskondft.append(res)         

koefon = np.array(maskondft)
koefoff = np.array(maskoffdft)
if u5_dft:
    koefcomp = np.array(maskoff_compare)

koefon = 10*np.log10(np.power(np.abs(koefon.T),2))
koefoff = 10*np.log10(np.power(np.abs(koefoff.T),2))
if u5_dft:
    koefcomp = 10*np.log10(np.power(np.abs(koefcomp.T),2))

if u5:
    yticks = np.linspace(0,512,9)
    ylabels = np.linspace(0,8000,9)

    xticks = np.linspace(0,100,11)
    xlabels = np.linspace(0,10,11)

    plt.figure(figsize=(21,11))
    plt.title("Spektrogram s rouškou")
    plt.xticks(ticks=xticks, labels=xlabels)
    plt.yticks(ticks=yticks, labels=ylabels)
    plt.xlabel("time [s]")
    plt.ylabel("freq [Hz]")
    plt.imshow(koefon[:512], origin="lower", aspect="auto")
    plt.colorbar()
    plt.figure(figsize=(21,11))
    plt.title("Spektrogram bez roušky")
    plt.xticks(ticks=xticks, labels=xlabels)
    plt.yticks(ticks=yticks, labels=ylabels)
    plt.xlabel("time [s]")
    plt.ylabel("freq [Hz]")
    plt.imshow(koefoff[:512], origin="lower", aspect="auto")
    plt.colorbar()
    plt.show()
    if u5_dft:
        plt.figure(figsize=(21,11))
        plt.title("Spektrogram bez roušky s vlastní implementací DFT")
        plt.xticks(ticks=xticks, labels=xlabels)
        plt.yticks(ticks=yticks, labels=ylabels)
        plt.xlabel("time [s]")
        plt.ylabel("freq [Hz]")
        plt.imshow(koefcomp[:512], origin="lower", aspect="auto")
        plt.colorbar()
        plt.show()

H = []
H_same = []
mon_same = np.array(maskondft_same)
moff_same = np.array(maskoffdft_same)
mon = np.array(maskondft)
moff = np.array(maskoffdft)
for x in range(len(maskoffdft)):
    H.append(mon[x]/moff[x])

for x in range(len(maskoffdft_same)):
    H_same.append(mon_same[x]/moff_same[x])
   
H = np.array(H)
H_same = np.array(H_same)

havg = np.average(np.abs(H),axis=0)
havg = havg[:512]

havg_same = np.average(np.abs(H_same),axis=0)
havg_same = havg_same[:512]

havgpow = 10*np.log10(np.power(np.real(havg[:512]),2))
havgpow_same = 10*np.log10(np.power(np.real(havg_same[:512]),2))

if u6:
    xaxis = np.linspace(0,8000,512)
    plt.plot(xaxis,havgpow)
    plt.title("Frekvenční charakteristika roušky")
    plt.xlabel("frekvence")
    plt.ylabel("hustota")
    plt.show()

if u13:
    xaxis = np.linspace(0,8000,512)
    plt.subplot(211)
    plt.plot(xaxis,havgpow)
    plt.title("Frekvenční charakteristika roušky")
    plt.xlabel("frekvence")
    plt.ylabel("hustota")
    plt.subplot(212)
    plt.plot(xaxis,havgpow_same)
    plt.title("Frekvenční charakteristika roušky při výběru shodných f0")
    plt.xlabel("frekvence")
    plt.ylabel("hustota")
    plt.show()

padding = [0 for _ in range(512)]
havg = np.concatenate((havg,padding))
odezva = np.fft.ifft(havg,1024)
odezva = odezva[:512]

havg_same = np.concatenate((havg_same,padding))
odezva_same = np.fft.ifft(havg_same,1024)
odezva_same = odezva_same[:512]

if u7:
    plt.plot(odezva)
    plt.title("Impulzní odezva rouškového filtru")
    plt.show()

if u7_idft:
    myodezva = myIDFT(havg)[:512]
    plt.plot(myodezva)
    plt.title("Impulzní odezva rouškového filtru s vlastní implementací IDFT")
    plt.show()

sf = scipy.signal.lfilter(odezva.real,[1.0],sentence) # aplikování filtru na nahrávku věty
sf = np.real(sf) # odstranění případné komplexní složky
sf = np.int16(sf) # převod na int16 pro export wav
wavfile.write("../audio/sim_maskon_sentence.wav",s_sr,sf) # export wav

st = scipy.signal.lfilter(odezva.real,[1.0],maskoff)
st = np.real(st)
st = np.int16(st)
wavfile.write("../audio/sim_maskon_tone.wav",moff_sr, st)

sf_same = scipy.signal.lfilter(odezva_same.real,[1.0],sentence)
sf_same = np.real(sf_same)
sf_same = np.int16(sf_same)

st_same = scipy.signal.lfilter(odezva_same.real,[1.0],maskoff)
st_same = np.real(st_same)
st_same = np.int16(st_same)

wavfile.write("../audio/sim_maskon_sentence_only_match.wav",s_sr,sf_same)
wavfile.write("../audio/sim_maskon_tone_only_match.wav",moff_sr, st_same)



if u8:
    plt.tight_layout()
    plt.subplots_adjust(hspace=1)
    plt.subplot(311)
    plt.title("Nahrávka věty bez roušky")
    plt.plot(sentence)
    plt.subplot(312)
    plt.title("Nahrávka věty s rouškou")
    plt.plot(sentenceon)
    plt.subplot(313)
    plt.title("Nahrávky věty se simulovanou rouškou")
    plt.plot(sf)
    plt.show()

#hpon hpoff

# Úkol 15

# Nové rámce - 25ms
dr_off = list()
dr_on = list()

for i in range(99):
    dr_on.append(hpon[i*160:int((i+2.5)*160)])
    dr_off.append(hpoff[i*160:int((i+2.5)*160)])

cutoff = []
cuton = []

posuny = []

for i in range(len(dr_off)):
    clipon = clip(dr_on[i])
    clipoff = clip(dr_off[i])
    
    w = np.concatenate((clipon,[0 for x in range(clipon.size)]))
    posunoffon = []
    for j in range(clipon.size):
         posunoffon.append(np.correlate(clipoff,w[j:j+clipon.size])[0])
    w = np.concatenate((clipoff,[0 for x in range(clipoff.size)]))
    posunonoff = []
    for j in range(clipoff.size):
        posunonoff.append(np.correlate(clipon,w[j:j+clipoff.size])[0])
    posunoff = np.array(posunoffon).argmax()
    posunon = np.array(posunonoff).argmax()

    if posunoff < posunon:
        posuny.append(posunoff)
        if posunoff == 0:
            cutoff.append(dr_off[i])
            cuton.append(dr_on[i])
        cutoff.append(dr_off[i][:-posunoff])
        cuton.append(dr_on[i][posunoff:])
    else:
        posuny.append(-posunon)
        if posunon == 0:
            cutoff.append(dr_off[i])
            cuton.append(dr_on[i])
        cutoff.append(dr_off[i][posunon:])
        cuton.append(dr_on[i][:-posunon])


for x in range(len(cutoff)):
    cutoff[x] = cutoff[x][:320]
    cuton[x] = cuton[x][:320]

if u15:
    plt.tight_layout()
    plt.title("Průběh fázového posunu mezi rámci")
    plt.ylabel("Posun [vzorky]")
    plt.xlabel("Rámec")
    plt.plot(posuny)
    plt.show()

    drawid = 1 

    plt.tight_layout()
    plt.subplots_adjust(hspace=1)
    plt.subplot(211)
    plt.title("Rámec 2 bez zarovnání")
    plt.plot(dr_off[drawid], label="bez roušky")
    plt.plot(dr_on[drawid], label="s rouškou")
    plt.legend()
    plt.subplot(212)
    plt.title("Rámec 2 se zarovnáním")
    plt.plot(cutoff[drawid], label="bez roušky")
    plt.plot(cuton[drawid], label="s rouškou")
    plt.legend()
    plt.show()

maskondft = []
maskoffdft = []
maskoff_compare = []
x = [0 for x in range(1024-ramce_moff[0].size)]
c = 0
for r in cutoff:
    ram = np.concatenate((r,x))
    res = np.fft.fft(ram,1024)
    c += 1
    maskoffdft.append(res)

c = 0
for r in cuton:
    ram = np.concatenate((r,x))
    res = np.fft.fft(ram,1024)
    c += 1
    maskondft.append(res)         

wav_from_dft(maskoffdft, maskondft, "_phase")

