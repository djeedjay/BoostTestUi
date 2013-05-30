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
		public void Wait5()
		{
			System.Console.WriteLine("Waiting");
			System.Threading.Thread.Sleep(1000);
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
}
