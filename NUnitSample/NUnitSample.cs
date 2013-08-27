namespace BoostTestUI
{
	using NUnit.Framework;

	[TestFixture]
	public class NUnitSample
	{
		[Test]
		public void Success()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		[Explicit]
		public void Fail()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		[Explicit]
		public void Exception()
		{
			throw new System.Exception("Failed Test");
		}

		[Test]
		[ExpectedException("System.Exception")]
		public void ExpectedException()
		{
			throw new System.Exception("Failed Test");
		}

		[Test]
		[Explicit]
		[ExpectedException("System.InvalidCastException")]
		public void ExpectedException2()
		{
			throw new System.Exception("Failed Test");
		}

		[Test]
		[Explicit]
		[ExpectedException("System.Exception")]
		public void NoExpectedException()
		{
		}

		[Test]
		[Explicit]
		public void NullException()
		{
			string name = null;
			name.IndexOf('x');
		}

		[Test]
		public void Wait1()
		{
			System.Console.WriteLine("Waiting");
			System.Threading.Thread.Sleep(1000);
		}

		[Test]
		public void Wait2()
		{
			System.Console.WriteLine("Line1");
			System.Console.WriteLine("Line2");
			System.Console.WriteLine("Waiting");
			System.Threading.Thread.Sleep(1000);
		}
	
		[Test]
		public void Wait3()
		{
			System.Console.WriteLine("Waiting");
			System.Threading.Thread.Sleep(1000);
		}
		
		[Test]
		public void Wait4()
		{
			System.Console.WriteLine("Waiting");
			System.Threading.Thread.Sleep(1000);
		}

		[Test]
		public void Wait5<T>([Values(1, "aap", false)] T t)
		{
			System.Console.WriteLine("Waiting: " + t.ToString());
			System.Threading.Thread.Sleep(1000);
		}

		[Test]
		public void Values([Values("one", "two", "three")] string s)
		{
			System.Console.WriteLine("Value: \"{0}\"", s);
		}

		[Test]
		public void MultiplyCombinatorial([Values(1, 2, 3)] double a, [Values(4, 5, 6)] double b)
		{
			Assert.AreEqual(a * b, b * a);
			System.Console.WriteLine("{0} x {1} = {2}", a, b, a * b);
		}

		[Test, Sequential]
		public void MultiplySequential([Values(1, 2, 3)] double a, [Values(4, 5, 6)] double b, [Values("Ann", "Bob")] string name)
		{
			Assert.AreEqual(a * b, b * a);
			System.Console.WriteLine("{0}: {1} x {2} = {3}", name, a, b, a * b);
		}

		[Test]
		[Ignore("ignored test")]
		public void IgnoredTest()
		{
			throw new System.Exception();
		}
	}

	[TestFixture]
	public class SecondSample
	{
		[Test]
		public void SecondTest()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}

	[TestFixture]
	public class ErrorInConstructor
	{
		public ErrorInConstructor()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		public void Test1()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		public void Test2()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}

	[TestFixture]
	public class ErrorInTestFixtureSetUp
	{
		[TestFixtureSetUp]
		public void TestFixtureSetUp()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		public void Test1()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		public void Test2()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}

	[TestFixture]
	public class ErrorInSetUp
	{
		[SetUp]
		public void SetUp()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		public void Test1()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		public void Test2()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}

	[TestFixture]
	public class ErrorInTearDown
	{
		[TearDown]
		public void TearDown()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		public void Test1()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		public void Test2()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}

	[TestFixture]
	public class ErrorInTestFixtureTearDown
	{
		[TestFixtureTearDown]
		public void TestFixtureTearDown()
		{
			Assert.AreEqual(1 + 1, 3);
		}

		[Test]
		public void Test1()
		{
			Assert.AreEqual(1 + 1, 2);
		}

		[Test]
		public void Test2()
		{
			Assert.AreEqual(1 + 1, 2);
		}
	}
}
