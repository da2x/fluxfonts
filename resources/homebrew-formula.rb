class Fluxfonts < Formula
  desc "a continual font generator for increased privacy"
  homepage "https://ctrl.blog/entry/fluxfonts"
  url "https://github.com/da2x/fluxfonts/archive/v2.0.tar.gz"
  sha256 "ed602102b3062e01ce422b6c685e2442f95134248f963f54a78b57f3cc618a50"

  def install
    system "make", "install",
                   "PREFIX=#{prefix}"
  end

  plist_options :manual => "fluxfontd"

  def plist; <<-EOS.undent
    <?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
    <plist version="1.0">
    <dict>
      <key>Label</key>
      <string>#{plist_name}</string>
      <key>ProgramArguments</key>
      <array>
        <string>#{opt_bin}/fluxfontd</string>
      </array>
      <key>RunAtLoad</key>
      <true/>
    </dict>
    </plist>
    EOS
  end

  test do
    system "fluxfontd"
  end
end
