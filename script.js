const menuToggle = document.querySelector('.menu-toggle');
const navLinks = document.querySelector('.nav-links');
const navItems = document.querySelectorAll('.nav-links a');
const revealElements = document.querySelectorAll('.reveal');
const statNumbers = document.querySelectorAll('.stat-number');
const contactForm = document.querySelector('.contact-form');
const topbar = document.querySelector('.topbar');

const toggleMenu = () => {
  navLinks.classList.toggle('active');
};

const closeMenu = () => {
  navLinks.classList.remove('active');
};

const setActiveNav = () => {
  const scrollPosition = window.scrollY + window.innerHeight / 3;
  document.querySelectorAll('section[id]').forEach((section) => {
    const sectionTop = section.offsetTop;
    const sectionHeight = section.offsetHeight;
    const id = section.getAttribute('id');
    const link = document.querySelector(`.nav-links a[href="#${id}"]`);

    if (link) {
      if (scrollPosition >= sectionTop && scrollPosition < sectionTop + sectionHeight) {
        navItems.forEach((item) => item.classList.remove('active'));
        link.classList.add('active');
      }
    }
  });
};

const revealObserver = new IntersectionObserver(
  (entries, observer) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        entry.target.classList.add('reveal-visible');
        observer.unobserve(entry.target);
      }
    });
  },
  {
    threshold: 0.2,
  }
);

revealElements.forEach((el) => revealObserver.observe(el));

const animateStats = () => {
  statNumbers.forEach((number) => {
    const target = Number(number.dataset.target) || 0;
    const duration = 1400;
    let start = 0;
    const stepTime = Math.max(Math.floor(duration / target), 20);

    const updateNumber = () => {
      start += 1;
      number.textContent = start;
      if (start < target) {
        window.requestAnimationFrame(updateNumber);
      } else {
        number.textContent = `${target}+`;
      }
    };

    if (target > 0) {
      updateNumber();
    }
  });
};

window.addEventListener('scroll', () => {
  setActiveNav();
  if (window.scrollY > 20) {
    topbar?.classList.add('scrolled');
  } else {
    topbar?.classList.remove('scrolled');
  }
});

window.addEventListener('load', () => {
  setActiveNav();
  animateStats();
});

if (menuToggle && navLinks) {
  menuToggle.addEventListener('click', toggleMenu);
  navItems.forEach((link) => {
    link.addEventListener('click', closeMenu);
  });
}

if (contactForm) {
  contactForm.addEventListener('submit', (event) => {
    event.preventDefault();
    window.alert('Thanks! I’ll be in touch soon.');
    contactForm.reset();
  });
}
