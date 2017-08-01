//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

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

            var filePathDll = System.String.Format(@"{0}\{1}.dll", path, filename);
            if (System.IO.File.Exists(filePathDll))
                return System.Reflection.Assembly.LoadFrom(filePathDll);

            var filePathExe = System.String.Format(@"{0}\{1}.exe", path, filename);
            if (System.IO.File.Exists(filePathExe))
                return System.Reflection.Assembly.LoadFrom(filePathExe);

            return null; // let the GAC try resolving it
        }
    }

}
