# Godwit radio research

Research behind proposed improvements to Godwit's AM/FM radio (`radio/` and `hmi/src/radio.*`). This branch is a working archive in a fork, not a proposed change to the project.

Start with:
1. [reports/VERIFICATION.md](reports/VERIFICATION.md): every claim checked against the code, measurements and primary sources. **Where it disagrees with a report, it wins.**
2. [RADIO-PLAN.md](RADIO-PLAN.md): the prioritized work plan (Phases 0–8).
3. [reports/SDR FM car radio receiver design.md](reports/SDR%20FM%20car%20radio%20receiver%20design.md): FM DSP, weak-signal handling, RTL-SDR limits, RDS/RBDS.
4. [reports/SDR hardware and AM reception.md](reports/SDR%20hardware%20and%20AM%20reception.md): receivers, front-end filtering, vehicle antennas, AM by direct sampling, NRSC-5 IBOC.

`research_notes/` holds the source notes the reports were written from, with citations. `tools/` holds the probe programs used for the measurements in the verification log. They build against `radio/src/fmreceiver.cpp`:

```sh
g++ -O2 -std=c++20 -Iradio/src DOCS/radio-research/tools/verify.cpp radio/src/fmreceiver.cpp -o /tmp/verify && /tmp/verify
bash DOCS/radio-research/tools/sinc.sh   # separation before/after the discriminator droop correction
```

Terminology: the digital radio system is called NRSC-5 / NRSC-5 IBOC / IBOC digital radio here. Its consumer brand name is a trademark of Xperi and is deliberately not used.

The reports were written with AI assistance (Claude Code) and then checked claim by claim, as recorded in the verification log. Figures marked as estimates or inferences in the notes have not been measured.
