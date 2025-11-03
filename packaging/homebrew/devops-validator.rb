class DevopsValidator < Formula
  desc "Multi-platform DevOps validation CLI tool"
  homepage "https://github.com/neonix888/devops-validator"
  url "https://github.com/neonix888/devops-validator/archive/v1.0.0.tar.gz"
  sha256 "PLACEHOLDER_SHA256"
  license "MIT"

  depends_on "cmake" => :build
  depends_on "nlohmann-json"
  depends_on "yaml-cpp"

  def install
    system "cmake", "-S", ".", "-B", "build",
                    "-DCMAKE_BUILD_TYPE=Release",
                    "-DCMAKE_INSTALL_PREFIX=#{prefix}",
                    "-DBUILD_TESTING=OFF"
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    system "#{bin}/devops-validator", "version"
    system "#{bin}/devops-validator", "help"
  end
end
