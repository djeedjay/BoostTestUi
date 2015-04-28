#region Copyright (c) 2008 by FEI Company

//
// All rights reserved. This file includes confidential and
// proprietary information of FEI Company.
//
// Created by: Claudia Peter
//

#endregion

using System.Collections.Generic;
using NUnit.Framework;

namespace FEI.Tem.Test.TestGFI
{
    public abstract class FixtureBase
    {
        // This class has default constructor so it will generate an error
    }

    /// <summary>
    /// Simple sequences of Set and Get calls on the GFI interface.
    /// (no checks on received notifications etc.)
    /// </summary>
    [TestFixture]
    public class TestGFI03SetGet : FixtureBase
    {
        [TestFixtureSetUp]
        public void TestFixtureSetUp()
        {
            System.Console.WriteLine("This is put as a test");
        }

        [TestFixtureTearDown]
        public void TestFixtureTearDown()
        {
            System.Console.WriteLine("This is put as a test");
        }

        [Test]
        public void TestEelsPixelSize()
        {
        }

        public void TestEelsPixelSize(int parameter)
        {
            // No attribute information available
        }
    }
}