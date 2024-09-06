import Lampenproblem.Formalisierung

def schwache_Vermutung_von_Taschner_Krabbenhoeft (n : Nat): Prop :=
  ∀ m: Nat, ∃ o:Nat, o > m → CheckPRFromStart n o
def starke_Vermutung_von_Taschner_Krabbenhoeft: Prop :=
  ∀ n: Nat, n ≥ 2 → schwache_Vermutung_von_Taschner_Krabbenhoeft n






--
