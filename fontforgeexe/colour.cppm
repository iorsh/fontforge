module;
#include <cstdint>
#include <algorithm>
#include <cmath>

using namespace std;

export module Colour;

export struct Colour {
   uint8_t a; // Order aiming to match 'Color' uint32_t type in gimage.h
   uint8_t r;
   uint8_t g;
   uint8_t b;

   explicit Colour(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=0) : a(a), r(r), g(g), b(b) {};

   [[nodiscard]] double R() const { return r / 255.0; }
   [[nodiscard]] double G() const { return g / 255.0; }
   [[nodiscard]] double B() const { return b / 255.0; }
   [[nodiscard]] double A() const { return a / 255.0; }

   [[nodiscard]]
   inline double H() const {
      double mx = max({R(), G(), B()});
      double mn = min({R(), G(), B()});
      double d = mx - mn;

      // Hue/Chroma meaningless if Saturation is 0...
      if (d == 0)
         return 0;

      if (R() >= mx)
         return 60 * fmod((G() - B()) / d, 6);

      if (G() >= mx)
         return 60 * ((B() - R()) / d + 2);

      return 60 * ((R() - G()) / d + 4);
   }

   [[nodiscard]]
   inline double Sl() const {
      double d = max({R(), G(), B()}) - min({R(), G(), B()});
      if (d == 0)
         return 0;
      return d / (1 - abs(2 * L() - 1));
   }

   [[nodiscard]]
   inline double L() const {
      return (max({R(), G(), B()}) + min({R(), G(), B()})) / 2.0;
   }

   [[nodiscard]]
   inline double Sv() const {
      double mx = max({R(), G(), B()});
      if (mx == 0)
         return 0;
      double mn = min({R(), G(), B()});
      return (mx - mn) / mx;
   }

   [[nodiscard]]
   inline double V() const {
      return max({R(), G(), B()});
   }
};