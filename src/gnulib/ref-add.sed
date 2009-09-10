/^# Packages using this file: / {
  s/# Packages using this file://
  ta
  :a
  s/ fcp / fcp /
  tb
  s/ $/ fcp /
  :b
  s/^/# Packages using this file:/
}
