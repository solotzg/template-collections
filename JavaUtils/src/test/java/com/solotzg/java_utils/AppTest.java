package com.solotzg.java_utils;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import java.math.BigInteger;
import java.util.Collections;
import java.util.Vector;

/**
 * Unit test for simple App.
 */
public class AppTest
        extends TestCase {
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest(String testName) {
        super(testName);
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite() {
        return new TestSuite(AppTest.class);
    }

    /**
     * Rigourous Test :-)
     */
    public void testApp() {

        MillerRabin millerRabin = new MillerRabin();
        assertTrue(millerRabin.isPrime(new BigInteger("87678262537778991782390312341223827409")));
        {
            Vector<BigInteger> factors = millerRabin
                    .findFactors(new BigInteger("87678262537778991782390312341223827397"));
            Collections.sort(factors);
            Vector<BigInteger> expect = new Vector<BigInteger>();
            {
                expect.add(new BigInteger("13"));
                expect.add(new BigInteger("13"));
                expect.add(new BigInteger("113"));
                expect.add(new BigInteger("147830009"));
                expect.add(new BigInteger("69535001581"));
                expect.add(new BigInteger("446643188084969"));
            }
            assertTrue(expect.equals(factors));
        }
    }
}
