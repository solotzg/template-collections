import java.io.FileInputStream;
import java.io.PrintStream;
import java.math.BigInteger;
import java.util.Random;
import java.util.Scanner;

public class Main {
    public static void main(String args[]) {
        Scanner cin = null;
        PrintStream ps = null;
        try {
            // cin = new Scanner(System.in);
            cin = new Scanner(new FileInputStream("in.txt"));
            //ps = new PrintStream(new FileOutputStream("out.txt"));
            //System.setOut(ps);
        } catch (Exception e) {
            return;
        }
        BigInteger n = cin.nextBigInteger();
        MillerRabin millerRabin = new MillerRabin();
        if (millerRabin.miller_rabin(n))
            System.out.println("Yes");
        else {
            millerRabin.findfac(n, 100);
            for (int i = 0; i < millerRabin.tol; ++i) {
                System.out.println(millerRabin.factor[i]);
            }
        }
    }
}

class MillerRabin {
    static final int Times = 20;

    // miller_rabin, return `true` if composite
    private boolean check(BigInteger a, BigInteger n, BigInteger x, int t) {
        BigInteger res = a.modPow(x, n);
        BigInteger last = res;
        for (int i = 1; i <= t; ++i) {
            res = res.multiply(res).mod(n);
            if (res.compareTo(BigInteger.ONE) == 0 && last.compareTo(BigInteger.ONE) != 0 && last.compareTo(n.subtract(BigInteger.ONE)) != 0)
                return true;
            last = res;
        }
        return res.compareTo(BigInteger.ONE) != 0;
    }

    // miller_rabin, return `true` if prime
    // max wrong rate: 1 / (4 ^ times)
    public boolean miller_rabin(BigInteger n) {
        if (n.compareTo(BigInteger.valueOf(2)) == 0) return true;
        if (n.compareTo(BigInteger.valueOf(2)) < 0) return false;
        if (!n.testBit(0)) return false;
        BigInteger x = n.subtract(BigInteger.ONE);
        int t = 0;
        while (!x.testBit(0)) {
            x = x.shiftRight(1);
            ++t;
        }
        int maxNumBitLength = 1 + n.bitLength();
        BigInteger nSub1 = n.subtract(BigInteger.ONE);
        for (int i = 0; i < Times; i++) {
            BigInteger a = new BigInteger(maxNumBitLength, new Random());
            a = a.mod(nSub1).add(BigInteger.ONE);
            if (check(a, n, x, t))
                return false; // composite
        }
        return true;
    }

    // pollard_rho
    public BigInteger factor[]; // factorization no order
    public int tol; // num of prime factor

    private BigInteger pollard_rho(BigInteger x, BigInteger c) {
        BigInteger i = BigInteger.ONE, k = BigInteger.valueOf(2);
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
    private void find_fac_imp(BigInteger n) {
        if (miller_rabin(n)) {
            factor[tol++] = n;
            return;
        }
        BigInteger p = n;
        while (p.compareTo(n) >= 0)
            p = pollard_rho(p, new BigInteger(n.bitLength() + 1, new Random()).mod(n.subtract(BigInteger.ONE)).add(BigInteger.ONE));
        find_fac_imp(p);
        find_fac_imp(n.divide(p));
    }

    void findfac(BigInteger n, int maxFactorNum) {
        this.tol = 0;
        factor = new BigInteger[maxFactorNum];
        find_fac_imp(n);
    }
}