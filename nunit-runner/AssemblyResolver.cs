//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.nunit.org/ for the NUnit test library home page.

namespace TestRunner
{
	class AssemblyResolver : System.IDisposable
	{
		private string path;

		public AssemblyResolver(string path)
		{
			this.path = path;
			System.AppDomain.CurrentDomain.AssemblyResolve += new System.ResolveEventHandler(OnAssemblyResolve);
		}

		public void Dispose()
		{
			System.AppDomain.CurrentDomain.AssemblyResolve -= new System.ResolveEventHandler(OnAssemblyResolve);
		}

		private System.Reflection.Assembly OnAssemblyResolve(object sender, System.ResolveEventArgs args)
		{
			var filename = args.Name.Substring(0, args.Name.IndexOf(","));
			return System.Reflection.Assembly.LoadFrom(path + "\\" + filename + ".dll");
		}
	}

}
