absolute(I, O) :- I<0, O = -I.
absolute(I, O) :- I>=0, O = I.

uloha23([], 0).
uloha23([X|TAIL], SOUCETA) :- absolute(X, V), uloha23(TAIL,PODSOUCET), SOUCETA is PODSOUCET+V. 
