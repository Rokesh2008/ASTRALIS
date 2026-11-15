import math, sys
MU=3.986004418e14; RE=6.371e6; G0=9.80665
def sim(stages, payload=2623.0, fairing=1900.0, h_t=400e3, verbose=False):
    r=RE; th=0; vr=0; vt=465.1*math.cos(math.radians(28.5))
    si=0; props=[s['prop'] for s in stages]; fairing_on=True; t=0; dt=0.05
    maxq=0; mecot=None
    def mass():
        m=payload+(fairing if fairing_on else 0)
        for i in range(si,len(stages)): m+=stages[i]['dry']+props[i]
        return m
    events=[]
    while t<1500:
        h=r-RE
        if h<-1: return dict(ok=False,why='crash',t=t)
        s=stages[si]; p=math.exp(-h/8500.0)
        T=s['tvac']-(s['tvac']-s['tsl'])*p; isp=s['ispvac']-(s['ispvac']-s['ispsl'])*p
        m=mass()
        thr=1.0
        if T/m>4*G0: thr=max(0.4,4*G0*m/T)
        T*=thr
        # guidance
        vrel_t=vt-7.2921e-5*r; v=math.hypot(vr,vrel_t)
        g=MU/r**2; cent=vt*vt/r
        if t<12: gam=90.0
        elif si==0:
            gam=90-62*min(1,(t-12)/140)**0.75
        else:
            w=0.012
            ar=w*w*(h_t-h)-2*w*vr+g-cent
            sg=max(-0.6,min(1.0,ar/(T/m)))
            gam=math.degrees(math.asin(sg))
        rho=1.225*p; q=0.5*rho*v*v; maxq=max(maxq,q)
        D=q*0.35*10.75
        Dr=D*vr/v if v>1 else 0; Dt=D*vrel_t/v if v>1 else 0
        if t<1 and T/m<g: return dict(ok=False,why='TWR<1',twr=T/m/g)
        ga=math.radians(gam)
        ar_=(T*math.sin(ga)-Dr)/m-g+cent
        at_=(T*math.cos(ga)-Dt)/m-vr*vt/r
        vr+=ar_*dt; vt+=at_*dt; r+=vr*dt; th+=vt/r*dt
        if h<0 and vr<0: r=RE; vr=0
        props[si]-=T/(isp*G0)*dt
        if fairing_on and h>110e3: fairing_on=False; events.append(('fairing',t,h))
        # orbit
        E=0.5*(vr*vr+vt*vt)-MU/r; hmom=r*vt
        a=-MU/(2*E) if E<0 else float('inf')
        e=math.sqrt(max(0,1+2*E*hmom*hmom/MU**2))
        rp=a*(1-e); ra=a*(1+e)
        if si==len(stages)-1 and rp-RE>=h_t-25e3:
            events.append(('MECO',t,h)); mecot=t; break
        if props[si]<=0:
            props[si]=0
            events.append(('BECO s%d'%(si+1),t,h,math.hypot(vr,vt)))
            if si<len(stages)-1: si+=1
            else: break
        t+=dt
    E=0.5*(vr*vr+vt*vt)-MU/r; hmom=r*vt
    a=-MU/(2*E) if E<0 else float('inf'); e=math.sqrt(max(0,1+2*E*hmom*hmom/MU**2))
    return dict(ok=mecot is not None, t=t, peri=(a*(1-e)-RE)/1e3, apo=(a*(1+e)-RE)/1e3,
        h=(r-RE)/1e3, v=math.hypot(vr,vt), propleft=props[-1], maxq=maxq/1e3, events=events, downrange=th*RE/1e3)
F9=[dict(dry=25600,prop=395700,tsl=9*845e3,tvac=9*914e3,ispsl=282,ispvac=311),
    dict(dry=3900,prop=92670,tsl=981e3,tvac=981e3,ispsl=348,ispvac=348)]
r=sim(F9); print(r)
# variant: RL10 upper stage swap (mass-adjusted)
v=[dict(F9[0]),dict(F9[1],dry=3900-490+168,tsl=110e3,tvac=110e3,ispsl=465,ispvac=465)]
print('RL10 S2',sim(v))
v=[dict(F9[0],dry=25600-9*470+9*3177,tsl=9*1860e3,tvac=9*2279e3,ispsl=366,ispvac=452),F9[1]]
print('RS25 S1',sim(v))
v=[dict(F9[0],dry=25600-9*470+9*168,tsl=0,tvac=9*110e3,ispsl=0,ispvac=465),F9[1]]
print('RL10 S1',sim(v))
