import Mathlib.Tactic.Find

-- n := Lampenanzahl
-- m := Anzahl Schritte
-- k := Rundenzahl


-- definiere eine hilfsfunktion, um ein einzelnes bit zu flippen. WTF WARUM IST DAS NICHT IN DER STANDARDBIBLOTHEK???????
def flip_at_helper {n: Nat} (m: Nat): (BitVec n) :=
  BitVec.ofNat n (2^m)

def all_on: BitVec n := BitVec.allOnes n
def all_off: BitVec n := BitVec.zero n
def flip_at {v: BitVec n} (m: Nat): (BitVec n) := BitVec.xor v (flip_at_helper ((m-1) % n))

inductive LampState: BitVec n  → (m: Nat) → (k: Nat) → Type where
  | start: LampState all_off 1 1
  | step:  LampState v m k → LampState (flip_at m) (m + k) (m/n + 1)
  deriving Repr

def BetterToStr (v: BitVec n) (i: Nat := n): String :=
  if i ≤ 0 then ""
  else
    if BitVec.getLsb v (n - i) = true then
      "☒" ++ BetterToStr v (i-1)
    else
      "☐" ++ BetterToStr v (i-1)

def PrintLampState (ls: @LampState n v m k): String :=
    "n:        " ++ ToString.toString n ++
  "\nDaten:    " ++ BetterToStr v ++
  "\nSchritte: " ++ ToString.toString m ++
  "\nRunden:   " ++ ToString.toString k ++ "\n"

def step_n_sum {n: Nat} {v: BitVec n} (NumSteps: Nat) (ls: LampState v m k):
  Σ (v': BitVec n) (m' k' : Nat), LampState v' m' k' :=
  match NumSteps with
  | 0 => ⟨_, _, _, ls⟩
  | Nat.succ x => match step_n_sum x ls with
    --| ⟨_, m', _, ls'⟩ => ⟨_, _, m'/n, ls'.step⟩
    | ⟨v', m', k', ls'⟩ => ⟨@flip_at n v' m', m' + k', m'/n + 1, ls'.step⟩
    --| ⟨v', m', k', ls'⟩ => ⟨@flip_at n v' (m' % n), m' + k', m'/n + 1, ls'.step⟩

def extract_ls (x: Σ (v': BitVec n) (m' k' : Nat), LampState v' m' k'):=
  x.snd.snd.snd



#print BetterToStr

#eval IO.println (PrintLampState (@LampState.start 7))
#eval IO.println (PrintLampState (step_n_sum 5 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (extract_ls (step_n_sum 50 (@LampState.start 7))))


#eval IO.println (PrintLampState (step_n_sum 0 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 1 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 2 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 3 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 4 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 5 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 6 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 7 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 8 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 9 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 10 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 11 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 12 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 13 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 14 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 15 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 16 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 17 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 18 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 19 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (step_n_sum 20 (@LampState.start 7)).snd.snd.snd)
--/

def positiveRunde  (_ : LampState v n m) : Prop := v = all_on ∨ v = all_off


--
