
// import java.io.FileInputStream;
// import java.io.PrintStream;
import java.math.BigInteger;
import java.util.Random;
import java.util.Scanner;
import java.util.Vector;

class MillerRabin {
    public static void main(String args[]) {
        Scanner cin = null;
        // PrintStream ps = null;
        try {
            cin = new Scanner(System.in);
            // cin = new Scanner(new FileInputStream("in.txt"));
            // ps = new PrintStream(new FileOutputStream("out.txt"));
            // System.setOut(ps);
        } catch (Exception e) {
            return;
        }
        BigInteger n = cin.nextBigInteger();
        MillerRabin millerRabin = new MillerRabin();
        if (millerRabin.isPrime(n))
            System.out.println("Yes");
        else {
            // 87678262537778991782390312341223827397 not prime
            // 87678262537778991782390312341223827409 prime
            Vector<BigInteger> factors;
            double timeCost;
            {
                final long startMillis = System.currentTimeMillis();
                factors = millerRabin.findFactors(n);
                timeCost = (System.currentTimeMillis() - startMillis) / 1000.0;
            }
            System.out.print("Not prime. Factors are:\n  ");
            System.out.println(factors);
            System.out.println(String.format("TimeCost: %.3fs", timeCost));
        }

        cin.close();
    }

    static final int Times = 20;

    // miller_rabin, return `false` if composite
    private boolean millerRabinTest(final BigInteger n, final BigInteger d, final int t) {
        BigInteger nSub1 = n.subtract(BigInteger.ONE);
        BigInteger a = null; // 2 ~ n-1
        {
            a = new BigInteger(1 + n.bitLength(), new Random());
            a = a.mod(n.subtract(BigInteger.TWO)).add(BigInteger.TWO);
        }
        BigInteger x = a.modPow(d, n);
        if (x.compareTo(BigInteger.ONE) == 0 || x.compareTo(nSub1) == 0)
            return true;
        for (int i = 0; i < t; ++i) {
            x = x.multiply(x).mod(n);
            if (x.compareTo(BigInteger.ONE) == 0)
                return false;
            if (x.compareTo(nSub1) == 0)
                return true;
        }
        return false;
    }

    // return `true` if prime
    // max wrong rate: 1 / (4 ^ times)
    public boolean isPrime(BigInteger n) {
        if (n.compareTo(BigInteger.TWO) == 0)
            return true;
        if (n.compareTo(BigInteger.TWO) < 0)
            return false;
        if (!n.testBit(0))
            return false;
        BigInteger d = n.subtract(BigInteger.ONE);
        int t = 0;
        while (!d.testBit(0)) {
            d = d.shiftRight(1);
            ++t;
        }
        for (int i = 0; i < Times; i++) {
            if (!millerRabinTest(n, d, t))
                return false; // composite
        }
        return true;
    }

    private BigInteger pollardRhoAlgorithm(BigInteger x, BigInteger c) {
        BigInteger i = BigInteger.ONE, k = BigInteger.TWO;
        BigInteger x0 = new BigInteger(x.bitLength() + 1, new Random()).mod(x);
        BigInteger y = x0;
        while (true) {
            i = i.add(BigInteger.ONE);
            x0 = x0.multiply(x0).add(c).mod(x);
            BigInteger d = y.subtract(x0).gcd(x);
            if (d.compareTo(BigInteger.ONE) != 0 && d.compareTo(x) != 0)
                return d;
            if (y.compareTo(x0) == 0)
                return x;
            if (i.compareTo(k) == 0) {
                y = x0;
                k = k.shiftLeft(1);
            }
        }
    }

    // factorize `n`
    private void findFactorsImpl(BigInteger n, Vector<BigInteger> factors) {
        if (isPrime(n)) {
            factors.add(n);
            return;
        }
        BigInteger p = n;
        while (p.compareTo(n) >= 0)
            p = pollardRhoAlgorithm(p, new BigInteger(n.bitLength() + 1, new Random()).mod(n.subtract(BigInteger.ONE))
                    .add(BigInteger.ONE));
        findFactorsImpl(p, factors);
        findFactorsImpl(n.divide(p), factors);
    }

    // pollard_rho: factorization no order
    public Vector<BigInteger> findFactors(BigInteger n) {
        Vector<BigInteger> factors = new Vector<>();
        findFactorsImpl(n, factors);
        return factors;
    }
}