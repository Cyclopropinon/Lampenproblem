import Mathlib.Tactic.Find

-- n := Lampenanzahl
-- m := Anzahl Schritte
-- k := Rundenzahl


-- definiere eine hilfsfunktion, um ein einzelnes bit zu flippen. WTF WARUM IST DAS NICHT IN DER STANDARDBIBLOTHEK???????
def flip_at_helper {n: Nat} (m: Nat): (BitVec n) :=
  BitVec.ofNat n (2^(m-1))

def all_on: BitVec n := BitVec.allOnes n
def all_off: BitVec n := BitVec.zero n
def flip_at {v: BitVec n} (m: Nat): (BitVec n) := BitVec.xor v (flip_at_helper m)

inductive LampState: BitVec n  → (m: Nat) → (k: Nat) → Type where
  | start: LampState all_off 1 1
  | step:  LampState v m k → LampState (flip_at m) (m + k) (m/n + 1)
  deriving Repr

def PrintLampState (ls: @LampState n v m k): String :=
  "n:          " ++ ToString.toString n ++
  "\nDaten:    " ++ ToString.toString v ++
  "\nSchritte: " ++ ToString.toString m ++
  "\nRunden:   " ++ ToString.toString k ++ "\n"

def step_n_sum {n: Nat} {v: BitVec n} (NumSteps: Nat) (ls: LampState v m k):
  Σ (v': BitVec n) (m' k' : Nat), LampState v' m' k' :=
  match NumSteps with
  | 0 => ⟨_, _, _, ls⟩
  | Nat.succ x => match step_n_sum x ls with
    --| ⟨_, m', _, ls'⟩ => ⟨_, _, m'/n, ls'.step⟩
    | ⟨v', m', k', ls'⟩ => ⟨@flip_at n v' m', m' + k', m'/n + 1, ls'.step⟩

def extract_ls (x: Σ (v': BitVec n) (m' k' : Nat), LampState v' m' k'):=
  x.snd.snd.snd



#eval IO.println (PrintLampState (@LampState.start 7))
#eval IO.println (PrintLampState (step_n_sum 5 (@LampState.start 7)).snd.snd.snd)
#eval IO.println (PrintLampState (extract_ls (step_n_sum 50 (@LampState.start 7))))

def positiveRunde  (_ : LampState v n m) : Prop := v = all_on ∨ v = all_off


--
