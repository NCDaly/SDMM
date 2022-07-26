# SDMM Final Presentation Notes - Nathan

## 1. Coding Theory and SDMM

- [responding to Bailey] So what does that look like? How do we do this?
- Bring together two seemingly opposite concepts: cryptographic secret sharing, and coding theory.
- Secret sharing is taking a message and splitting it up into pieces, such that you need to combine a certain number of pieces to learn anything at all about the messsage.
  - examples: "two-man rule" in nuclear launch sequences
- Coding theory is taking a message and encoding it into a longer codeword, such that even if part of the codeword is lost you can still recover the message in full.
  - examples: checksums, credit card numbers, ISBNs
- These seem contradictory: one makes the message harder to recover, one makes it easier to recover.
- Actually just two ways of looking at the same question: how much information do we need to recover the message?
  - secret sharing: not enough people ==> not enough info
  - coding theory: single codeword ==> more than enough info
- Right at the intersection is the polynomial code
  - it's a superhero! (because it has a secret identity)
  - cryptographers call it "Shamir's secret sharing"
  - coding theorists call it the "Reed-Solomon code"
  - exact same math!
  
- Example: you're going on a nice vacation in the mountains. Cabin phone number: 123-456-7890
  - first 7 digits known - how to distribute the last 3 among your three friends?
  - you don't want to be bothered, but they should be able to reach you in an emergency
  - Idea 1:
    - write each digit on a slip of paper (i.e. "8__", "_9_", "__0") and give one to each friend
    - problem (coding theorists): if one friend loses their slip, they can't reach you
  - Idea 2:
    - write two digits on each slip (i.e. "89_", "_90", "8_0") and give one to each friend
    - problem (cryptographers): if one friend wants to prank call you, it only takes ten guesses
  - Idea 3: (polynomial codes)
    - plot your number on the y-axis of a Cartesian plane
    - draw a random line through that point
    - give each friend a different point on that line
    - coding theorists happy: if any one friend loses their point, the other two are enough to recover your number
    - cryptographers happy: no one friend gains any info about your number - infinite # of lines through that point
    
- How to extend this?
  - Share with more friends: easy, just send more points
  - Require more shares to reconstruct: a little harder
    - How many points define a parabola? Three! A cubic? Four!
    - A degree $n$ polynomial is uniquely defined by $n + 1$ points
    - Proof: interpolating a degree $n$ polynomial = solving a sys.eq. in $n + 1$ variables
    - I can make a random degree $n$ polynomial and distribute points from that
    - So anyone with $n$ or fewer points knows nothing about the y-intercept
  - Share multiple numbers: harder, but still doable
    - first number --> y-intercept ($x^0$ coefficient)
    - second number --> $x$ coefficient
    - other coeffs still random
    - still takes $n + 1$ to reconstruct the full polynomial
    - but with $n$ points, I know the relation between the coeffs - not good from a security pov!
    - (solving sys.eq. with n_eqs = n_vars = 1: can't quite get a point but can get a line)
    - So number of colluding friends successfully blocked = number of random coefficients
    
- But polynomial codes are all about numbers, and matrices aren't numbers, right? What gives?
  - First, we need to talk about fields
    - Not that kind! A field is a set of objects with addition, subtraction, multiplication, division
    - Ex: Rational numbers are a field, integers aren't b/c division not closed, integers mod 7 are
    - Polynomial codes actually work for any field
  - Once we have a field, we can define a vector space over that field
    - A vector space is a nice way of combining two different types of mathematical objects
    - One type, the "vectors", is anything you can add and subtract
    - The other type, the "scalars", is your field
    - Vector space means you can multiply vectors by scalars
  - Matrices of field elements form a vector space over that field
  - That's what lets us build polynomial codes using matrices as coefficients

- Now let's put this all together: the Secure Mat-Dot algorithm (what we implemented this summer)
  - Example: yo want to multiply two 2x2 matrices $A$ and $B$
    - Let's split $A$ into two columns ($A_1$ and $A_2$), $B$ into two rows ($B_1$ and $B_2$)
    - So you can write the product as $AB = A_1 B_1 + A_2 B_2$
    - Let's make polynomial codes for (the pieces of) $A$ and $B$:
      - $f(x) = A_1 + A_2 x + R x^2$, where $R$ is a ranodm 2x1 matrix
      - $g(x) = B_2 + B_1 x + S x^2$, where $S$ is a random 1x2 matrix
      - note: each of these functions returns a matrix
      - remember, we're in a vector space!
    - What happens when we multiply these?
      - $h(x) = (fg)(x) = A_1 B_2 + (A_1 B_2 + A_2 B_1)x + (A_1 S + A_2 B_1 + R B_2)x^2 + (A_2 S + R B_1)x^3 + RSx^4$
      - that's interesting... the $x$ coefficient is exactly equal to $AB$!
      - $h$ is in fact a polynomial code for $AB$
      - great feature of poly codes: can create new polynomials by adding/multiplying existing ones
    - you can give each friend $i$ the info $(x_i, f(x_i), g(x_i))$
      - since they only have one point, they learn nothing about $f$ or $g$
      - but they can multiply these matrices to get $h(x_i)$ and send it back
    - Now you have points of $h(x)$; you can use them to recover $AB$
      - how many do you need? Since $h$ has degree 4, need $4 + 1 = 5$ points
      - if you have five friends and they all chip in, you're all set; if one is busy, you're stuck
      - but if you have six friends... remember, any five points will do, So even if one friend's busy you're still good!
    - And there's the algo! Can change it around to suit your needs
      - Want to split your matrices into 3, 4, 40 parts? You can do that!
        - Smaller multiplications ==> less work for your friends :)
	- More terms ==> need more friends, more points ==> more work for you :(
      - Dont trust your friends? Afraid two of them will share their points
        - Just add more random terms!
	- Again, more terms ==> need more friends, more work :(
    - This seems like more work than just doing it yourself! A few considerations:
      - As matrices get bigger, adding does get harder, but multiplying gets WAY harder!
        - Doing more additions may be an acceptable tradeoff if it means smaller (faster) multiplications
      - Maybe LinAlg isn't your strong suit; maybe your friends are matrix multiplication whizzes
        - SDMM can give you access to compute power far exceeding your own

- That's the big picture of SDMM, how it works and why it's useful
- Now let's talk about what we did this summer to make it happen

## 2. Implementation: Interpolation