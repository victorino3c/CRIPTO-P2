def parse_args():
    parser = argparse.ArgumentParser(description="Comprobación de Seguridad Perfecta del cifrado Afín.")
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-P', action='store_true', help="Usa el método equiprobable.")
    group.add_argument('-I', action='store_true', help="Usa el método no equiprobable.")
    
    parser.add_argument('-i', '--input', metavar='FILE', type=str,
                        help="Archivo de entrada")
    parser.add_argument('-o', '--output', metavar='FILE',
                        type=str, help="Archivo de salida")
    
    return parser.parse_args()

class KeyGenerator:
    def _init_(self, m: int = 26, weights: list = None):
        self.m = m
        self.a_values = [x for x in range(m)
                         if euclidean_algorithm(x, m)[-1] == 1]
        print(f"m: {m}, a_values: {self.a_values}")
        if weights is None:
            self.weights = [(2*i)**2 for i in range(1, self.m+1)]
            #self.weights = [0]*(m-1) + [1]
        else:
            self.weights = weights
        
    def get_next_key(self, mode: str = 'P'):
        if mode == 'P':
            return self.generate_key_equiprobable()
        elif mode == 'I':
            return self.generate_key_inequiprobable()
        else:
            raise ValueError("Invalid mode. Must be 'P' or 'I'")

    def generate_key_equiprobable(self):
        return random.choice(self.a_values), random.randint(0, self.m-1)
    
    def generate_key_inequiprobable(self):
        a = random.choices(self.a_values,
                           weights=self.weights[:len(self.a_values)])[0]
        b = random.choices(range(self.m), weights=self.weights[:self.m])[0]
        return a, b

def calculate_probabilities(text) -> dict[str, float]:
    """Calcula las probabilidades de los caracteres en el texto.

    Args:
        text (str): Texto a analizar.

    Returns:
        dict[str, float]: Diccionario con las probabilidades de los caracteres.
    """
    counts = Counter(text)
    total = sum(counts.values())
    probabilities = {char: 100.0 * counts[char] / total for char in sorted(counts)}

    # Comprobar que la suma de las probabilidades es 1
    if round(sum(probabilities.values()), 10) != 100.0:
        s = sum(probabilities.values())
        raise ValueError(f"Probabilities do not sum to 100. {s}")

    return probabilities

def calculate_conditional_probabilities(plaintext, ciphertext) -> dict[str, dict[str, float]]:
    """Calcula las probabilidades condicionadas de los caracteres en el texto
    cifrado respecto al texto plano.

    Args:
        plaintext (str): Texto plano.
        ciphertext (str): Texto cifrado.

    Returns:
        dict[str, dict[str, float]]: Diccionario con las probabilidades condicionadas.
    """
    # joint_counts = defaultdict(Counter)
    # for pt_char, ct_char in zip(plaintext, ciphertext):
    #     joint_counts[pt_char][ct_char] += 1

    # conditional_probs = {}
    # for pt_char in sorted(joint_counts):
    #     total = sum(joint_counts[pt_char].values())
    #     conditional_probs[pt_char] = {ct_char: 100.0 * joint_counts[pt_char][ct_char] / total 
    #                                   for ct_char in sorted(joint_counts[pt_char])}
        
    #     # Comprobar que la suma de las probabilidades es 100
    #     if round(sum(conditional_probs[pt_char].values()), 10) != 100.0:
    #         s = sum(conditional_probs[pt_char].values())
    #         raise ValueError(f"Probabilities of {pt_char} do not sum to 100. {s}")
        
    # joint_counts = defaultdict(Counter)
    # for ct_char, pt_char in zip(ciphertext, plaintext):
    #     joint_counts[ct_char][pt_char] += 1

    # conditional_probs = {}
    # for ct_char in sorted(joint_counts):
    #     total = sum(joint_counts[ct_char].values())
    #     conditional_probs[ct_char] = {pt_char: 100.0 * joint_counts[ct_char][pt_char] / total 
    #                                   for pt_char in sorted(joint_counts[ct_char])}
        
    #     # Comprobar que la suma de las probabilidades es 100
    #     if round(sum(conditional_probs[ct_char].values()), 10) != 100.0:
    #         s = sum(conditional_probs[ct_char].values())
    #         raise ValueError(f"Probabilities of {ct_char} do not sum to 100. {s}")
        
    joint_counts = defaultdict(Counter)
    joint_counts2 = defaultdict(Counter)
    for pt_char, ct_char in zip(plaintext, ciphertext):
        joint_counts[pt_char][ct_char] += 1
        joint_counts2[ct_char][pt_char] += 1

    conditional_probs_xy = {}
    conditional_probs_yx = {}
    for pt_char in sorted(joint_counts):
        total = sum(joint_counts[pt_char].values())
        conditional_probs_xy[pt_char] = {ct_char: (100.0 * joint_counts[pt_char][ct_char] 
                                                / sum(joint_counts2[ct_char].values())) 
                                      for ct_char in sorted(joint_counts[pt_char])}
        conditional_probs_yx[pt_char] = {ct_char: 100.0 * joint_counts[pt_char][ct_char] / total 
                                           for ct_char in sorted(joint_counts[pt_char])}
                                        
        # Comprobar que la suma de las probabilidades es 100
        if round(sum(conditional_probs_yx[pt_char].values()), 10) != 100.0:
            s = sum(conditional_probs_yx[pt_char].values())
            raise ValueError(f"Probabilities of {pt_char} do not sum to 100. {s}")

    return conditional_probs_xy